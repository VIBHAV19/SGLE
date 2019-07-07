#pragma once
#include "glFramework.h"
class Button : public Entity, public EventManager {
public:
	Button(int x, int y, std::string label);
	void draw(glWindow* wnd);
	void update(glWindow* wnd, long dT);
};

