#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

// Check for Vulkan Api errors in Debug Mode
#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

namespace VkEngine {
    inline constexpr const char* ENGINE_NAME = "Generic_Vk_Engine";
    inline constexpr uint32_t ENGINE_VK_VERSION = VK_API_VERSION_1_3;
    inline constexpr uint32_t ENGINE_VERSION = VK_MAKE_API_VERSION(0, 0, 1, 0);
    const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};
}