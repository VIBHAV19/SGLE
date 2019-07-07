#include "stdafx.h"
#include "Button.h"

Button::Button(int x, int y, std::string label) {
	this->x = x;
	this->y = y;
}

void Button::draw(glWindow* wnd) {
	double xp, yp;
	wnd->pix2Pos(x, y, &xp, &yp);
	glRectd(xp, yp, xp + 0.1, yp + 0.1);
}

void Button::update(glWindow* wnd, long dT) {

}