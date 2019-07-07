#pragma once

#include <map>

#include <SDL2/SDL_keyboard.h>

enum Event {
	MOUSEMOVE,
	MOUSEDOWN,
	MOUSECLICK,
	MOUSESCROLL,
	KEYUP,
	KEYDOWN,
	KEYPRESS,
	WINDOWRESIZE,
	CLICK,

	TRANSITION,
	OTHER,
	LOADED
};


class EventArgs {};

class WindowEvent : public EventArgs {
public:
	int x, y, width, height;
};

class KeyEvent : public EventArgs {
public:
	SDL_Keycode key;
	bool controlDown, shiftDown, altDown;
};

class MouseEvent : public EventArgs {
public:
	int x, y;
	bool lDown, rDown, mDown;
};

class MouseScrollEvent : public MouseEvent {
public:
	int scrollXDist, scrollYDist;
};

class LoadedEvent : public EventArgs {
public:
	uint8_t *data;
};

typedef void(*Callback)(EventArgs&);


class EventManager {
	std::map<Event, Callback> events;
public:
	void addEventListener(Event event, Callback callback);
	void removeEventListener(Event event);
	void removeAllEvents();
protected:
	void fireEvent(Event e, EventArgs& eventArgs);
};
