#pragma once
#include "EventManager.h"
#include "glFramework.h"
#include "glUtils.h"

enum TransitionPos {
	START,
	HALF,
	FINISHED
};

class TransitionEvent : public EventArgs {
public:
	TransitionPos pos = START;
};

class Transition : public EventManager, public Entity {
private:
	int posx = 0;
	bool going = false;
	bool half = false;
	double speed = 2.0;

	GLuint fadeTex;
	glImage fadeImg;

public:
	Transition() {}

	Transition(const char* transMask);

	void update(glWindow *wnd, long dT);
	void draw(glWindow *wnd);
	void go(double speed = 2.0);
};

