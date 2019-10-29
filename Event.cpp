#include "Event.h"
#include "stdafx.h"

const char* Event::LOADED = "loaded";

const char* WindowEvent::RESIZE = "windowResize";

const char* MouseEvent::MOUSE_MOVE = "mouseMove";
const char* MouseEvent::MOUSE_DOWN = "mouseDown";
const char* MouseEvent::CLICK = "mouseClick";

const char* KeyboardEvent::KEY_DOWN = "keyDown";
const char* KeyboardEvent::KEY_PRESS = "keyPress";
const char* KeyboardEvent::KEY_UP = "keyUp";