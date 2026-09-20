
#include "../../include/Graphics/GPUManager.hpp"
#include <stdexcept>


VkEngine::GPUManager::GPUManager(VkInstance* instance, VkSurfaceKHR* surface): instance(instance), surface(surface){}

VkEngine::GPUManager::~GPUManager(){}

void VkEngine::GPUManager::findAvailableGPUs()
{
    // First check if there are any compatible devices
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(*instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("[Device Error]: Failed to find a GPU with Vulkan support");
    }

    // Store all available devices
    availableGPUs.resize(deviceCount);
    vkEnumeratePhysicalDevices(*instance, &deviceCount, availableGPUs.data());
}

std::vector<VkPhysicalDevice> VkEngine::GPUManager::getAvailableGPUs()
{
    return this->getAvailableGPUs();
}

// TODO ignore GPU that over no swapchain, queues, preset support
// If all gpus are unsupported throw error
void VkEngine::GPUManager::pickBestGPU()
{
    if(this->availableGPUs.empty())
    {
        throw std::runtime_error("[Device Error]: Cannot pick GPU. No GPU listed");
    }

    VkPhysicalDevice chosenGPU = VK_NULL_HANDLE;
    int score = -1;
    for(auto &gpu: this->availableGPUs)
    {
        int gpuScore = this->scoreGPU(gpu);
        if(gpuScore > score)
        {
            chosenGPU = gpu;
            score = gpuScore;
        }
    }

    this->pickGPU(chosenGPU);
}

int VkEngine::GPUManager::scoreGPU(VkPhysicalDevice gpu)
{
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(gpu, &properties);

    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(gpu, &features);

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

    score += static_cast<int>(properties.limits.maxImageDimension2D); // Rate similar devices by the texture resolution

    return score;
}

void VkEngine::GPUManager::pickGPU(VkPhysicalDevice gpu)
{

}

VkPhysicalDeviceProperties VkEngine::GPUManager::getGPUProperties(VkPhysicalDevice gpu) const
{
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(gpu, &properties);
    return properties;
}

VkPhysicalDeviceFeatures VkEngine::GPUManager::getGPUFeatures(VkPhysicalDevice gpu) const
{
    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(gpu, &features);
    return features;
}