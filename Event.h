#pragma once

#include <SDL2/SDL_keyboard.h>

class Event {
public:
	static const char* LOADED;
};

class WindowEvent : public Event {
public:
	int x, y, width, height;

	static const char* RESIZE;
};

class MouseEvent : public Event {
public:
	int x, y;

	static const char* MOUSE_MOVE;
	static const char* MOUSE_DOWN;
	static const char* CLICK;
};

class KeyboardEvent : public Event {
public:
	SDL_Keycode key;
	bool controlDown, shiftDown, altDown;

	static const char* KEY_DOWN;
	static const char* KEY_PRESS;
	static const char* KEY_UP;
};

