#include "stdafx.h"
#include "Transition.h"

Transition::Transition(const char* transMask) {
	fadeImg = objModel::loadImage(transMask);
	fadeTex = objModel::createTexture(fadeImg);

	posx = 0;
	going = false;
	half = false;
}

void Transition::update(glWindow* wnd, long dT) {
	if (going) {
		posx += dT * speed;

		if (posx > wnd->winWidth * 3 && !half) {
			TransitionEvent f;
			f.pos = TransitionPos::HALF;
			fireEvent(TRANSITION, f);
			half = true;
		}

		if (posx > wnd->winWidth * 6) {
			TransitionEvent f;
			f.pos = TransitionPos::FINISHED;
			fireEvent(TRANSITION, f);

			posx = 0;
			going = false;
			half = false;
		}
	}
}

void Transition::draw(glWindow* wnd) {
	if (going && visible) {
		glUseProgram(0);
		wnd->projection_ortho();
		wnd->drawRectTex(fadeTex, wnd->winWidth - posx, 0, wnd->winWidth * 6 - posx, wnd->winHeight);
	}
}

void Transition::go(double speed) {
	if (!going) {
		going = true;
		TransitionEvent f;
		f.pos = TransitionPos::START;
		fireEvent(TRANSITION, f);
		this->speed = speed;
	}
}