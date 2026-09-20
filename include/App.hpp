#include "../include/Constants/EngineConstants.hpp"
#include "./Graphics/VkWindow.hpp"

namespace VkEngine {
    class Application {
    public:
        Application(VkWindow* window, const char *appName, uint32_t appVersion = VK_MAKE_API_VERSION(0, 1, 0, 0));
        ~Application();

    private:
        //Steps for building the app
        void createVkInstance();
        void createVkSurface();

        void addValidationLayersOnDebug(VkInstanceCreateInfo &createInfo);
        bool checkValidationLayerSupport() const;
        VkInstance instance = VK_NULL_HANDLE;
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        VkWindow* window = nullptr;
        const char *appName = nullptr;
        uint32_t appVersion;
    };
}
