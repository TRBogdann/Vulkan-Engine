#pragma once
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

namespace VkEngine{
    class GPUManager
    {
        public:
            GPUManager(VkInstance* instance, VkSurfaceKHR* surface);
            ~GPUManager();
            void findAvailableGPUs();
            std::vector<VkPhysicalDevice> getAvailableGPUs();
            void pickBestGPU();
            void pickGPU(VkPhysicalDevice gpu);
            int scoreGPU(VkPhysicalDevice gpu);
            VkPhysicalDeviceProperties getGPUProperties(VkPhysicalDevice gpu) const;
            VkPhysicalDeviceFeatures getGPUFeatures(VkPhysicalDevice gpu) const;
        
        private:
            VkInstance* instance;
            VkSurfaceKHR* surface;
            std::vector<VkPhysicalDevice> availableGPUs;
            VkPhysicalDevice currentGPU = VK_NULL_HANDLE;
            VkDevice logicalGPU = VK_NULL_HANDLE;
    };
}