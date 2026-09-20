#pragma once
#include "../Events/Event.hpp"
#include <GLFW/glfw3.h>
#include <string>

// TODO Implement other screen options

// Full screen
// GLFWwindow* window = glfwCreateWindow(640, 480, "My Title", NULL, NULL);

// Borderless
/*
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

glfwWindowHint(GLFW_RED_BITS, mode->redBits);
glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "My Title",
monitor, NULL);
 */

namespace VkEngine {
    // Wrapper over GLFWWindow that adds window control through events
    class VkWindow {
        public:
            VkWindow(const std::string &title, int width, int height);
            ~VkWindow();
            GLFWwindow *getGLFWWindow();
            int getHeight();
            int getWidth();
            float getAspectRatio();
            bool isClosed();
            void setColor(unsigned char r, unsigned char g, unsigned char b, unsigned char alpha);
            void destroy();
            void clear();
            void swap();
            bool wasDestroyed();
            void pollEvents(Event &event);
            // void setIcon(std::string path); TODO

            friend class Application;

        private:
            void handleWindowEvents(Event &event);
            bool resizable;
            GLFWwindow *window;
            bool closed;
            int type;
            int width;
            int height;
            std::string title;
            float r, g, b, a;
            bool destroyed = 0;
    };
}