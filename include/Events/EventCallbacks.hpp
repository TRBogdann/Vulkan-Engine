#pragma once

#include <GLFW/glfw3.h>

namespace VkEngine
{
    void windowFbSizeCallback(GLFWwindow *window, int width, int height);
    void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    void cursorPositionCallback(GLFWwindow *window, double xpos, double ypos);
    void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
    void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);
    void windowCloseCallback(GLFWwindow *window);
    void windowMaximizeCallback(GLFWwindow *window, int maximized);
    void windowIconifyCallback(GLFWwindow *window, int iconified);
    void windowFocusCallback(GLFWwindow *window, int focused);
}