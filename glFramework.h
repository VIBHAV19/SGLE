#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <functional>
#include <map>
#include <Windows.h>
#include <gl/glew.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "EventManager.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>

class objModel;
class glWindow;

class Entity {
public:
	static std::vector<Entity*> entities;
	int x = 0, y = 0;
	int zIndex = 0;
	bool visible = true;
public:
	virtual void draw(glWindow *wnd) = 0;
	virtual void update(glWindow* wnd, long dT) = 0;
	virtual int audioCallback(uint8_t* data, int len, SDL_AudioSpec audioSpec) {
		return 0;
	}
};

namespace Key {
	bool isDown(uint8_t key);
}

class glWindow : public EventManager {
	float FOV = 45.0f;
	float nearPoint = 0.4f;
	float farPoint = 500.0f;

	bool dontQuit = false;

	SDL_Window *mainwindow;
	SDL_GLContext maincontext;
	GLuint shaderID;

	void handleEvent(SDL_Event e);
	int InitGL();
	void loadDefShaders();

	static SDL_AudioSpec wantedSpec;
	static void audioMixer(void* userdata, Uint8* stream, int len);
public:
	glm::mat4 Projection;
	glm::mat4 View;
	glm::vec3 Camera;
	unsigned int winWidth;
	unsigned int winHeight;

	struct {
		std::map<SDL_Keycode, bool> keys;
		bool isDown(SDL_Keycode keyCode) {
			std::map<SDL_Keycode, bool>::iterator it;
			it = keys.find(keyCode);
			if (it != keys.end())
				return it->second;
			return false;
		}
	} Key;

	struct {
		int x, y;
		bool lButton, rButton, mButton;
	} Mouse;

	glWindow();
	glWindow(const char* title, int width, int height, bool fullscreenflag);
	
	~glWindow();

	void addChild(Entity *e);

	GLvoid ReSizeGLScene(GLsizei, GLsizei);

	void setView(float, float, float);
	void updateCamera();
	void swapBuffers();

	void projection_perspective();
	void projection_ortho();

	void lookAt(glm::vec3, glm::vec3);
	void setFog(float);

	void drawRectTex(GLuint texture, int x1, int y1, int x2, int y2, double texU1 = 0.0, double texV1 = 0.0, double texU2 = 1.0, double texV2 = 1.0);
	void drawRectTex(GLuint texture, int x1, int y1, int x2, int y2, glm::vec4 color, double texU1 = 0.0, double texV1 = 0.0, double texU2 = 1.0, double texV2 = 1.0);
	void pix2Pos(int x, int y, double* xo, double* yo);
	void drawRectTex(GLuint texture, int x1, int y1, int x2, int y2, glm::vec4 color1, glm::vec4 color2, double texU1 = 0.0, double texV1 = 0.0, double texU2 = 1.0, double texV2 = 1.0);
	
	void gameLoop(void(*update)(long dT), void(*glRender)());
	
	static GLuint LoadShaders(const char*, const char*);

	static GLuint CompileShaders(const char* vertex_file_path, const char* fragment_file_path);

	void useShader(GLuint program);
	void useDefaultShader();

	friend class objModel;
};
