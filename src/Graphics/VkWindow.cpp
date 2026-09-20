#include "../../include/Graphics/VkWindow.hpp"
#include "../../include/Events/EventCallbacks.hpp"
#include <GLFW/glfw3.h>
#include <stdexcept>


VkEngine::VkWindow::VkWindow(const std::string &title, int width, int height)
    : title(title),
      width(width),
      height(height),
      closed(0),
      r(0.0f),
      g(0.0f),
      b(0.0f),
      a(0.0f),
      type(0),
      resizable(1) 
{
    if(glfwInit() != GLFW_TRUE)
    {
        throw std::runtime_error("[GLFW Error]: Could not init GLFW");
    }
    
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    this->window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);

    if (this->window == NULL)
    {
        throw std::runtime_error("[GLFW Error]: Could not create Window");
    }

    glfwSetKeyCallback(this->window, keyCallback);
    glfwSetCursorPosCallback(this->window, cursorPositionCallback);
    glfwSetMouseButtonCallback(this->window, mouseButtonCallback);
    glfwSetScrollCallback(this->window, scrollCallback);
    glfwSetWindowCloseCallback(this->window, windowCloseCallback);
    glfwSetWindowMaximizeCallback(this->window, windowMaximizeCallback);
    glfwSetWindowIconifyCallback(this->window, windowIconifyCallback);
    glfwSetWindowFocusCallback(this->window, windowFocusCallback);
    glfwSetFramebufferSizeCallback(this->window, windowFbSizeCallback);
}

VkEngine::VkWindow::~VkWindow()
{
    if (!this->destroyed)
    {
        glfwDestroyWindow(this->window);
        glfwTerminate();
    }
}

void VkEngine::VkWindow::clear() 
{
    // Will probably be handled by Vulkan
}

void VkEngine::VkWindow::setColor(unsigned char r, unsigned char g, unsigned char b, unsigned char alpha)
{
    this->r = float(r) / 255.0f;
    this->g = float(g) / 255.0f;
    this->b = float(b) / 255.0f;
    this->a = float(alpha) / 255.0f;
}

void VkEngine::VkWindow::destroy()
{
    this->closed = 1;
    this->destroyed = 1;
    glfwDestroyWindow(this->window);
}

void VkEngine::VkWindow::handleWindowEvents(Event &event)
{
    if (event.triggered && event.window.triggered) {
        switch (event.window.event) {
            case EventType::WindowResized:
            case EventType::WindowRestored:
            case EventType::WindowMaximized:
                this->width  = event.window.data1;
                this->height = event.window.data2;
                break;

            case EventType::WindowClosed:
                this->closed = 1;
                break;

            default:
                break;
        }
    }
}

void VkEngine::VkWindow::pollEvents(Event &event) {
    glfwSetWindowUserPointer(window, &event);

    event.reset();
    glfwPollEvents();

    this->handleWindowEvents(event);
}

void VkEngine::VkWindow::swap() { /* handled by vkQueuePresentKHR later */ }

float VkEngine::VkWindow::getAspectRatio() { return float(this->height) / float(this->width);}

bool VkEngine::VkWindow::isClosed() { return this->closed; }

int VkEngine::VkWindow::getHeight() { return this->height; }

int VkEngine::VkWindow::getWidth() { return this->width; }

GLFWwindow* VkEngine::VkWindow::getGLFWWindow() {return this->window; }

bool VkEngine::VkWindow::wasDestroyed() { return this->destroyed; }