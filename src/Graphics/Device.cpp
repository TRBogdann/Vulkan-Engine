#include "../../include/Graphics/Device.hpp"
#include <set>
#include <stdexcept>

VkEngine::Device::Device(VkInstance instance, VkSurfaceKHR surface): instance(instance), surface(surface)
{
    enumeratePhysicalDevices();
    pickPhysicalDevice();
    createLogicalDevice();
}

VkEngine::Device::~Device()
{
    if (device != VK_NULL_HANDLE) {
        vkDestroyDevice(device, nullptr);
    }
    // physicalDevice is not owned/destroyed — it's tied to the instance
}

void VkEngine::Device::enumeratePhysicalDevices()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("[Device Error]: Failed to find a GPU with Vulkan support");
    }

    availableDevices.resize(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, availableDevices.data());
}

std::vector<VkPhysicalDevice> VkEngine::Device::getAvailableDevices() const
{
    return this->availableDevices;
}

void VkEngine::Device::pickPhysicalDevice()
{
    if (this->availableDevices.empty()) {
        throw std::runtime_error("[Device Error]: Cannot pick device. No devices enumerated");
    }

    VkPhysicalDevice chosenDevice = VK_NULL_HANDLE;
    int bestScore = -1;
    QueueFamilyIndices chosenIndices;
    SwapChainSupportDetails chosenSwapChainSupport;

    for (const auto &candidate : this->availableDevices) {
        QueueFamilyIndices indices = this->findQueueFamilies(candidate);
        SwapChainSupportDetails swapChainSupport = this->querySwapChainSupport(candidate);

        if (!this->isDeviceSuitable(candidate, indices, swapChainSupport)) {
            continue;
        }

        int score = this->scoreDevice(candidate);
        if (score > bestScore) {
            chosenDevice = candidate;
            bestScore = score;
            chosenIndices = indices;
            chosenSwapChainSupport = swapChainSupport;
        }
    }

    if (chosenDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("[Device Error]: No supported GPUs found. Are you running this on a coffee machine?");
    }

    this->bindPhysicalDevice(chosenDevice, chosenIndices, chosenSwapChainSupport);
}

VkResult VkEngine::Device::selectPhysicalDevice(VkPhysicalDevice physDevice)
{
    QueueFamilyIndices indices = this->findQueueFamilies(physDevice);
    SwapChainSupportDetails swapChainSupport = this->querySwapChainSupport(physDevice);

    if (!this->isDeviceSuitable(physDevice, indices, swapChainSupport)) {
        return VK_ERROR_FEATURE_NOT_PRESENT;
    }

    this->bindPhysicalDevice(physDevice, indices, swapChainSupport);
    return VK_SUCCESS;
}

int VkEngine::Device::scoreDevice(VkPhysicalDevice physDevice) const
{
    VkPhysicalDeviceProperties properties = this->getDeviceProperties(physDevice);

    int score = 0;

    switch (properties.deviceType) {
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            score += 1000;
            break;
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            score += 100;
            break;
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
            score += 10;
            break;
        case VK_PHYSICAL_DEVICE_TYPE_CPU:
        default:
            score += 1;
            break;
    }

    // Rate similar devices by max texture resolution as a rough capability tiebreaker
    score += static_cast<int>(properties.limits.maxImageDimension2D);

    return score;
}

bool VkEngine::Device::isDeviceSuitable(VkPhysicalDevice physDevice, const QueueFamilyIndices &indices, const SwapChainSupportDetails &swapChainSupport) const
{
    bool extensionsSupported = this->checkDeviceExtensionSupport(physDevice);
    bool swapChainAdequate = extensionsSupported && !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();

    return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

void VkEngine::Device::bindPhysicalDevice(VkPhysicalDevice physDevice, const QueueFamilyIndices &indices, const SwapChainSupportDetails &swapChainSupport)
{
    this->physicalDevice = physDevice;
    this->queueFamilyIndices = indices;
    this->swapChainSupportDetails = swapChainSupport;
}

void VkEngine::Device::createLogicalDevice()
{
    std::set<uint32_t> uniqueQueueFamilies = {
        this->queueFamilyIndices.graphicsFamily.value(),
        this->queueFamilyIndices.presentFamily.value()
    };

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    float queuePriority = 1.0f;

    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(VkEngine::requiredGPUExtensions.size());
    createInfo.ppEnabledExtensionNames = VkEngine::requiredGPUExtensions.data();

    // Modern Vulkan ignores device-level validation layers (instance-level is enough),
    // but setting them keeps compatibility with older implementations.
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(VkEngine::validationLayers.size());
        createInfo.ppEnabledLayerNames = VkEngine::validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(this->physicalDevice, &createInfo, nullptr, &this->device) != VK_SUCCESS) {
        throw std::runtime_error("[Device Error]: Failed to create logical device");
    }

    vkGetDeviceQueue(this->device, this->queueFamilyIndices.graphicsFamily.value(), 0, &this->graphicsQueue);
    vkGetDeviceQueue(this->device, this->queueFamilyIndices.presentFamily.value(), 0, &this->presentQueue);
}

QueueFamilyIndices VkEngine::Device::findQueueFamilies(VkPhysicalDevice physDevice) const
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physDevice, &queueFamilyCount, queueFamilies.data());

    for (uint32_t i = 0; i < queueFamilyCount; i++) {
        // BIT MAGIC TRICK!
        // queueFlags is used as an array of bits where every bit represents whether the queue supports a certain operation or not.
        // VK_QUEUE_GRAPHICS_BIT contains 1 inside the bit that represents its support index.
        // If the queue doesn't support graphical operations the bit will be 0 and the bitwise and will always result in 0.
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(physDevice, i, this->surface, &presentSupport);
        if (presentSupport) {
            indices.presentFamily = i;
        }

        // We found queues for both graphics and present operations.
        // Since we use only one queue per operation (for now at least) we stop searching.
        if (indices.isComplete()) {
            break;
        }
    }

    return indices;
}

SwapChainSupportDetails VkEngine::Device::querySwapChainSupport(VkPhysicalDevice physDevice) const
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physDevice, this->surface, &details.capabilities);

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physDevice, this->surface, &formatCount, nullptr);
    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physDevice, this->surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physDevice, this->surface, &presentModeCount, nullptr);
    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physDevice, this->surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

bool VkEngine::Device::checkDeviceExtensionSupport(VkPhysicalDevice physDevice) const
{
    uint32_t extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(physDevice, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(physDevice, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(VkEngine::requiredGPUExtensions.begin(), VkEngine::requiredGPUExtensions.end());

    for (const auto &extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

VkPhysicalDeviceProperties VkEngine::Device::getDeviceProperties(VkPhysicalDevice physDevice) const
{
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(physDevice, &properties);
    return properties;
}

VkPhysicalDeviceFeatures VkEngine::Device::getDeviceFeatures(VkPhysicalDevice physDevice) const
{
    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(physDevice, &features);
    return features;
}