#pragma once
#include "./KeyList.hpp"
#include <cstdint>

namespace VkEngine {
enum class InputState { Released = 0, Pressed = 1, Repeat = 2 };

enum class EventType : uint32_t {
  NullEvent = 0,

  WindowNullEvent = 0x200,
  WindowResized = 0x201,
  WindowClosed = 0x202,
  WindowMinimized = 0x203,
  WindowMaximized = 0x204,
  WindowFocused = 0x205,
  WindowNotFocused = 0x206,
  WindowHidden = 0x207,
  WindowShown = 0x208,
  WindowRestored = 0x209,

  MouseMotion = 111,
  WheelScroll = 222
};

enum class MouseButton {
  Button1 = 0,
  Button2 = 1,
  Button3 = 2,
  Button4 = 3,
  Button5 = 4,
  Button6 = 5,
  Button7 = 6,
  Button8 = 7,
  Last = Button8,
  Left = Button1,
  Right = Button2,
  Middle = Button3
};

// --- Structs, updated to use the enums ---

struct WindowEvent {
  bool triggered = false;
  int action = 0;
  int windowID = 0;
  EventType event = EventType::WindowNullEvent;
  int data1 = 0;
  int data2 = 0;
};

struct Keysym {
  Key key = Key::Unknown;
  int scancode = 0;
  int mods = 0;
};

struct KeyboardEvent {
  bool triggered = false;
  int action = 0;
  int windowID = 0;
  InputState state = InputState::Released;
  short repeat = 0;
  Keysym keysym;
};

struct MouseMotionEvent {
  bool triggered = false;
  int action = 0;
  int timestamp = 0;
  int windowID = 0; /**< The window with mouse focus, if any */
  int which = 0;    /**< The mouse instance id */
  InputState state = InputState::Released;
  int x = 0;    /**< X coordinate, relative to window */
  int y = 0;    /**< Y coordinate, relative to window */
  int xrel = 0; /**< The relative motion in the X direction */
  int yrel = 0; /**< The relative motion in the Y direction */
};

struct MouseButtonEvent {
  bool triggered = false;
  int action = 0;
  int timestamp = 0;
  int windowID = 0; /**< The window with mouse focus, if any */
  int which = 0;    /**< The mouse instance id */
  MouseButton button = MouseButton::Left;
  InputState state = InputState::Released;
  int x = 0; /**< X coordinate, relative to window */
  int y = 0; /**< Y coordinate, relative to window */
};

struct MouseWheelEvent {
  bool triggered = false;
  int action = 0;
  int timestamp = 0;
  int windowID = 0; /**< The window with mouse focus, if any */
  int which = 0;    /**< The mouse instance id */
  int x = 0;        /**< The amount scrolled horizontally */
  int y = 0;        /**< The amount scrolled vertically */
};

struct Event {
  unsigned int event_ID;
  bool triggered = false;
  WindowEvent window;
  KeyboardEvent key;
  MouseWheelEvent wheel;
  MouseButtonEvent button;
  MouseMotionEvent motion;

  inline void reset() {
    triggered = false;
    window.triggered = false;
    key.triggered = false;
    motion.triggered = false;
    button.triggered = false;
    wheel.triggered = false;
  }

  friend class Application;
};
}