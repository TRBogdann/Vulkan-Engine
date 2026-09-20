#pragma once
#include "../Constants/EngineConstants.hpp"
#include "DeviceExtensions.hpp"

namespace VkEngine {

    class Device
    {
        public:
            Device(VkInstance instance, VkSurfaceKHR surface);
            ~Device();

            // Non-copyable: VkDevice ownership shouldn't be duplicated
            Device(const Device &) = delete;
            Device&operator=(const Device &) = delete;

            void enumeratePhysicalDevices();
            std::vector<VkPhysicalDevice> getAvailableDevices() const;

            void pickPhysicalDevice();
            VkResult selectPhysicalDevice(VkPhysicalDevice physDevice);
            void createLogicalDevice();

            int scoreDevice(VkPhysicalDevice physDevice) const;
            bool isDeviceSuitable(VkPhysicalDevice physDevice, const QueueFamilyIndices &indices, const SwapChainSupportDetails &swapChainSupport) const;

            VkPhysicalDeviceProperties getDeviceProperties(VkPhysicalDevice physDevice) const;
            VkPhysicalDeviceFeatures getDeviceFeatures(VkPhysicalDevice physDevice) const;
            QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physDevice) const;
            SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice physDevice) const;

            VkPhysicalDevice getPhysicalDevice() const { return physicalDevice; }
            VkDevice getDevice() const { return device; }
            VkQueue getGraphicsQueue() const { return graphicsQueue; }
            VkQueue getPresentQueue() const { return presentQueue; }
            const QueueFamilyIndices &getQueueFamilyIndices() const { return queueFamilyIndices; }
            const SwapChainSupportDetails &getSwapChainSupportDetails() const { return swapChainSupportDetails; }

        private:
            void bindPhysicalDevice(VkPhysicalDevice physDevice, const QueueFamilyIndices &indices, const SwapChainSupportDetails &swapChainSupport);
            bool checkDeviceExtensionSupport(VkPhysicalDevice physDevice) const;

            VkInstance instance = VK_NULL_HANDLE;   // non-owning, borrowed from Application
            VkSurfaceKHR surface = VK_NULL_HANDLE;  // non-owning, borrowed from Application

            std::vector<VkPhysicalDevice> availableDevices; // all enumerated devices, non-owning handles

            VkPhysicalDevice physicalDevice = VK_NULL_HANDLE; // chosen device, non-owning
            VkDevice device = VK_NULL_HANDLE;                 // owning, destroyed in ~Device

            VkQueue graphicsQueue = VK_NULL_HANDLE;
            VkQueue presentQueue = VK_NULL_HANDLE;

            QueueFamilyIndices queueFamilyIndices;
            SwapChainSupportDetails swapChainSupportDetails;
    };
}