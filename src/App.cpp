#include "../include/App.hpp"
#include <GLFW/glfw3.h>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <vector>

VkEngine::Application::Application(VkWindow* window, const char *appName, uint32_t appVersion)
{ 
    this->appName = appName;
    this->appVersion = appVersion;
    this->window = window;

    createVkInstance();
    createVkSurface();
    this->device = std::make_unique<Device>(instance, surface);
}

void VkEngine::Application::createVkInstance()
{
    // I. App Info
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = this->appName;
    appInfo.applicationVersion = this->appVersion;
    appInfo.pEngineName = VkEngine::ENGINE_NAME;
    appInfo.engineVersion = VkEngine::ENGINE_VERSION;
    appInfo.apiVersion = VkEngine::ENGINE_VK_VERSION;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    // II. Enabling GLFW extensions
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    if (glfwExtensions == nullptr)
    {
        throw std::runtime_error("Failed to get required GLFW Vulkan extensions");
    }

    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;

    // III. Add validation layers when running in debug
    createInfo.enabledLayerCount = 0;
    this->addValidationLayersOnDebug(createInfo);

    // IV. Create the actual vulkan instance
    if (vkCreateInstance(&createInfo, nullptr, &this->instance) != VK_SUCCESS)
    {
        throw std::runtime_error("[Failed to create application]: Could not create Vulkan Instance");
    }
}

void VkEngine::Application::createVkSurface()
{
    if (glfwCreateWindowSurface(instance, window->getGLFWWindow(), nullptr, &surface) != VK_SUCCESS)
    {
        throw std::runtime_error("[Failed to create application]: Could not create Vulkan Surface");
    }
}

VkEngine::Application::~Application()
{
    device.reset();
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(this->instance, nullptr);
}

bool VkEngine::Application::checkValidationLayerSupport() const
{
    uint32_t layerCount;
    bool support = true;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char *layerName : VkEngine::validationLayers)
    {
        bool layerFound = false;
        for (const auto &layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
            return false;
        }
    }

    return true;
}

void VkEngine::Application::addValidationLayersOnDebug(VkInstanceCreateInfo &createInfo)
{
    if (!enableValidationLayers)
    {
        return;
    }

    if (!checkValidationLayerSupport())
    {
        std::cerr << "[Debug Warning]: Vulkan validation is enabled, but is not supported";
        return;
    }

    createInfo.enabledLayerCount = static_cast<uint32_t>(VkEngine::validationLayers.size());
    createInfo.ppEnabledLayerNames = VkEngine::validationLayers.data();
}
