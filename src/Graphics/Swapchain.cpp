#include "../../include/Graphics/SwapChain.hpp"
#include <algorithm>
#include <limits>
#include <stdexcept>


VkEngine::SwapChain::SwapChain(Device* device, VkSurfaceKHR surface, VkWindow* window)
    : device(device), surface(surface), window(window)
{
    create();
    createImageViews();
}

VkEngine::SwapChain::~SwapChain()
{
    cleanup();
}

void VkEngine::SwapChain::cleanup()
{
    for (auto imageView : this->imageViews) {
        vkDestroyImageView(this->device->getDevice(), imageView, nullptr);
    }
    this->imageViews.clear();

    if(this->swapChain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(this->device->getDevice(), this->swapChain, nullptr);
        this->swapChain = VK_NULL_HANDLE;
    }
}

void VkEngine::SwapChain::create()
{
    SwapChainSupportDetails swapChainSupport = this->device->querySwapChainSupport(this->device->getPhysicalDevice());

    VkSurfaceFormatKHR surfaceFormat = chooseSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = choosePresentMode(swapChainSupport.presentModes);
    VkExtent2D chosenExtent = chooseExtent(swapChainSupport.capabilities);

    // Set the number of images we use in the buffer
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0) {
        imageCount = std::min(imageCount, swapChainSupport.capabilities.maxImageCount);
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = this->surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = chosenExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    const QueueFamilyIndices &indices = this->device->getQueueFamilyIndices();
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    if (indices.graphicsFamily != indices.presentFamily) {
        // An image can be own by a single queue familly
        // We must tranfer the ownership to another queue before each operation
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    // We do not want to apply any image transformations for now (like rotating or flipping the image)
    // So the next line is the equivalent of "don't do any transform" (we still need to initialise the variable)
    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(this->device->getDevice(), &createInfo, nullptr, &this->swapChain) != VK_SUCCESS) {
        throw std::runtime_error("[Vulkan Error]: Failed to create swapchain");
    }

    // Retrieve the actual images (driver may create more than requested)
    uint32_t actualImageCount = 0;
    vkGetSwapchainImagesKHR(this->device->getDevice(), this->swapChain, &actualImageCount, nullptr);
    this->images.resize(actualImageCount);
    vkGetSwapchainImagesKHR(this->device->getDevice(), this->swapChain, &actualImageCount, this->images.data());

    this->imageFormat = surfaceFormat.format;
    this->extent = chosenExtent;
}


void VkEngine::SwapChain::createImageViews()
{
    this->imageViews.resize(images.size());

    for (size_t i = 0; i < this->images.size(); i++) {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = images[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = imageFormat;
        viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device->getDevice(), &viewInfo, nullptr, &imageViews[i]) != VK_SUCCESS) {
            throw std::runtime_error("[Vulkan Error]: Failed to create image view");
        }
    }
}

VkSurfaceFormatKHR VkEngine::SwapChain::chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) const
{
    for (const auto &availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }
    return availableFormats[0]; // fallback — guaranteed non-empty by Device::isDeviceSuitable
}

// TODO - allow this to be chosen from settings
VkPresentModeKHR VkEngine::SwapChain::choosePresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes) const
{
    for (const auto &availablePresentMode : availablePresentModes) {
        // Use triple buffering if it's available
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR; // will always be available for suitable devices
}

VkExtent2D VkEngine::SwapChain::chooseExtent(const VkSurfaceCapabilitiesKHR &capabilities) const
{
    // We need the window dimensions for buffer alocation so we ask the window manager for the dimensions 
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }

    // If the window manager did not return any dimenions we use the ones set in the app
    int width = this->window->getWidth();
    int height = this->window->getHeight();

    VkExtent2D actualExtent = {
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height)
    };

    actualExtent.width = std::clamp(
        actualExtent.width,
        capabilities.minImageExtent.width,
        capabilities.maxImageExtent.width
    );

    actualExtent.height = std::clamp(
        actualExtent.height,
        capabilities.minImageExtent.height, 
        capabilities.maxImageExtent.height
    );

    return actualExtent;
}