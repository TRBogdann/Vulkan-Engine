#include "../../include/Events/EventCallbacks.hpp"
#include "../../include/Events/Event.hpp"

namespace VkEngine {
    void window_fbsize_callback(GLFWwindow *window, int width, int height)
    {
        Event *event = (Event *)glfwGetWindowUserPointer(window);
        if(!event) return; // User didn't provide any adress so we can't store the event data
        event->triggered = true;
        event->window.triggered = true;
        event->window.event = EventType::WindowResized;
        event->window.data1 = width;
        event->window.data2 = height;
    }

    void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        Event *event = (Event *)glfwGetWindowUserPointer(window);
        if(!event) return;
        event->triggered = true;
        event->key.triggered = true;
        event->key.state = static_cast<InputState>(action);
        event->key.keysym.scancode = scancode;
        event->key.keysym.key = static_cast<Key>(key);
        event->key.keysym.mods = mods;
    }

    void cursor_position_callback(GLFWwindow *window, double xpos, double ypos)
    {
        Event *event = (Event *)glfwGetWindowUserPointer(window);
        if(!event) return;
        event->triggered = true;
        event->motion.triggered = true;
        event->motion.x = static_cast<int>(xpos);
        event->motion.y = static_cast<int>(ypos);
    }

    void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
    {
        Event *event = (Event *)glfwGetWindowUserPointer(window);
        if(!event) return;
        event->triggered = true;
        event->button.triggered = true;
        event->button.button = static_cast<MouseButton>(button);
        event->button.state = static_cast<InputState>(action);
    }

    void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
    {
        Event *event = (Event *)glfwGetWindowUserPointer(window);
        if(!event) return;
        event->triggered = true;
        event->wheel.triggered = true;
        event->wheel.x = static_cast<int>(xoffset);
        event->wheel.y = static_cast<int>(yoffset);
    }

    void window_close_callback(GLFWwindow *window)
    {
        //By default close button has no functionality. Let the event propagate before trying to store the date
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        Event *event = (Event *)glfwGetWindowUserPointer(window);
        if (!event) return;
        event->triggered = true;
        event->window.triggered = true;
        event->window.event = EventType::WindowClosed;
    }

    void window_maximize_callback(GLFWwindow *window, int maximized)
    {
        Event *event = (Event *)glfwGetWindowUserPointer(window);
        if(!event) return;
        event->triggered = true;
        event->window.triggered = true;
        glfwGetWindowSize(window, &event->window.data1, &event->window.data2);
        if (maximized) {
            event->window.event = EventType::WindowMaximized;
        } else {
            event->window.event = EventType::WindowRestored;
        }
    }

    void window_iconify_callback(GLFWwindow *window, int iconified)
    {
        Event *event = (Event *)glfwGetWindowUserPointer(window);
        if(!event) return;
        event->triggered = true;
        event->window.triggered = true;
        glfwGetWindowSize(window, &event->window.data1, &event->window.data2);
        if (iconified) {
            event->window.event = EventType::WindowMinimized;
        } else {
            event->window.event = EventType::WindowRestored;
        }
    }

    void window_focus_callback(GLFWwindow *window, int focused)
    {
        Event *event = (Event *)glfwGetWindowUserPointer(window);
        if(!event) return;
        event->triggered = true;
        event->window.triggered = true;
        if (focused){
            event->window.event = EventType::WindowFocused;
        } else {
            event->window.event = EventType::WindowNotFocused;
        }
    }
}