#pragma once
#include "Device.hpp"
#include "VkWindow.hpp"
#include <vector>

namespace VkEngine {

class SwapChain {
    public:
        SwapChain(Device* device, VkSurfaceKHR surface, VkWindow* window);
        ~SwapChain();

        SwapChain(const SwapChain &) = delete;
        SwapChain &operator=(const SwapChain &) = delete;

        void recreate();

        VkSwapchainKHR getSwapChain() const { return swapChain; }
        VkFormat getImageFormat() const { return imageFormat; }
        VkExtent2D getExtent() const { return extent; }
        const std::vector<VkImage>& getImages() const { return images; }
        const std::vector<VkImageView>& getImageViews() const { return imageViews; }
        size_t getImageCount() const { return images.size(); }

    private:
        void create();
        void cleanup();

        VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) const;
        VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes) const;
        VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR &capabilities) const;

        void createImageViews();

        Device* device = nullptr;         // non-owning, borrowed from Application
        VkSurfaceKHR surface = VK_NULL_HANDLE; // non-owning, borrowed from Application
        VkWindow* window = nullptr;       // non-owning, borrowed from Application

        VkSwapchainKHR swapChain = VK_NULL_HANDLE; // owning
        std::vector<VkImage> images;               // non-owning — owned by the swapchain itself
        std::vector<VkImageView> imageViews;       // owning — created and destroyed by this class

        VkFormat imageFormat = VK_FORMAT_UNDEFINED;
        VkExtent2D extent{};
};

}