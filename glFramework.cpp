#include "stdafx.h"
#include "glFramework.h"

const char *vtxShader =
#include "Shaders/MainVtx.shd"
;

const char *frgShader =
#include "Shaders/MainFrag.shd"
;

#define MAX_LOADSTRING 65536

std::vector<Entity*> Entity::entities;
SDL_AudioSpec glWindow::wantedSpec;

GLvoid glWindow::ReSizeGLScene(GLsizei width, GLsizei height) {
	if (height == 0)
		height = 1;

	winWidth = width;
	winHeight = height;

	//Projection transform
	Projection = glm::perspective(glm::radians(FOV), (float)width / (float)height, nearPoint, farPoint);
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FOV, (GLfloat)width / (GLfloat)height, nearPoint, farPoint);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int glWindow::InitGL() {
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDepthFunc(GL_LEQUAL);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glewInit();

	return TRUE;
}

void glWindow::setView(float fov, float nearPt, float farPt) {
	FOV = fov;
	nearPoint = nearPt;
	farPoint = farPt;
}

glWindow::~glWindow() {
	SDL_GL_DeleteContext(maincontext);
	SDL_DestroyWindow(mainwindow);
	SDL_Quit();
}

void glWindow::addChild(Entity* e) {
	Entity::entities.push_back(e);
}

glWindow::glWindow() {}

glWindow::glWindow(const char* title, int width, int height, bool fullscreenflag) {
	if (SDL_WasInit(SDL_INIT_VIDEO) == 0) {
		if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0)
			return;
	}
	if (SDL_WasInit(SDL_INIT_AUDIO) == 0) {
		if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
			return;

		wantedSpec.freq = 48000;
		wantedSpec.format = AUDIO_U16;
		wantedSpec.channels = 2;
		wantedSpec.samples = 4096;
		wantedSpec.callback = audioMixer;
		wantedSpec.userdata = NULL;

		if (SDL_OpenAudio(&wantedSpec, NULL) < 0) {
			std::cout << "Couldn't open audio: " << SDL_GetError() << std::endl;
			return;
		}

		SDL_PauseAudio(0);
	}
	
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	
	mainwindow = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | (fullscreenflag ? SDL_WINDOW_FULLSCREEN : 0));

	if (!mainwindow)
		return;

	maincontext = SDL_GL_CreateContext(mainwindow);

	SDL_GL_MakeCurrent(mainwindow, maincontext);

	//V-Sync
	SDL_GL_SetSwapInterval(1);

	if (!InitGL()) {
		MessageBoxA(NULL, "Initialization Failed.", "ERROR", MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	loadDefShaders();

	ReSizeGLScene(width, height);
}

void glWindow::loadDefShaders() {
	this->shaderID = CompileShaders(vtxShader, frgShader);
}

void glWindow::audioMixer(void* userdata, Uint8* stream, int len) {
	uint8_t* data = (uint8_t*)SDL_malloc(len);
	SDL_memset(stream, 0, len);
	SDL_memset(data, 0, len);

	for (Entity *c : Entity::entities) {
		if (c != nullptr) {
			int volume = c->audioCallback(data, len, glWindow::wantedSpec);
			SDL_MixAudio(stream, data, len, volume);
		}
	}

	SDL_free(data);
}

GLuint glWindow::LoadShaders(const char * vertex_file_path, const char * fragment_file_path) {
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	} else {
		printf("Impossible to open %s. Are you in the right directory ?\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	return CompileShaders(VertexShaderCode.c_str(), FragmentShaderCode.c_str());
}


GLuint glWindow::CompileShaders(const char* ctxSrcCode, const char* fgmtSrcCode) {
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	glShaderSource(VertexShaderID, 1, &ctxSrcCode, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	glShaderSource(FragmentShaderID, 1, &fgmtSrcCode, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}


	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

void glWindow::useShader(GLuint program) {
	glUseProgram(program);
}
void glWindow::useDefaultShader() {
	glUseProgram(this->shaderID);
}
void glWindow::updateCamera() {
	glUniform3f(glGetUniformLocation(this->shaderID, "C"), Camera.x, Camera.y, Camera.z);
}

void glWindow::lookAt(glm::vec3 location, glm::vec3 upVec) {
	this->View = glm::lookAt(this->Camera, location, upVec);
}

void glWindow::setFog(float density) {
	glUniform1f(glGetUniformLocation(this->shaderID, "fogDensity"), density);
}

void glWindow::swapBuffers() {
	SDL_GL_SwapWindow(mainwindow);
}

void glWindow::handleEvent(SDL_Event e) {
	switch (e.type) {
	case SDL_QUIT:
		dontQuit = false;
		break;
	case SDL_WINDOWEVENT:
		switch (e.window.event) {
		case SDL_WINDOWEVENT_RESIZED:
			ReSizeGLScene(e.window.data1, e.window.data2);
			WindowEvent ev;
			ev.width = e.window.data1;
			ev.height = e.window.data2;
			fireEvent(Event::WINDOWRESIZE, ev);
			break;
		}
		break;
	case SDL_KEYDOWN:
	{
		KeyEvent ev;
		ev.key = e.key.keysym.sym;
		ev.controlDown = e.key.keysym.mod & KMOD_CTRL;
		ev.altDown = e.key.keysym.mod & KMOD_ALT;
		ev.shiftDown = e.key.keysym.mod & KMOD_SHIFT;

		Key.keys.insert_or_assign(ev.key, true);

		if (!e.key.repeat)
			fireEvent(Event::KEYPRESS, ev);
		fireEvent(Event::KEYDOWN, ev);
	}
	break;

	case SDL_KEYUP:
	{
		KeyEvent ev;
		ev.key = e.key.keysym.sym;
		Key.keys.insert_or_assign(ev.key, false);
		fireEvent(Event::KEYUP, ev);
	}
	break;

	case SDL_MOUSEMOTION:
	{
		MouseEvent ev;
		Mouse.x = e.motion.x;
		Mouse.y = e.motion.y;

		ev.x = Mouse.x;
		ev.y = Mouse.y;

		fireEvent(Event::MOUSEMOVE, ev);
	}
		break;
	case SDL_MOUSEBUTTONDOWN:
	{
		MouseEvent ev;
		Mouse.x = e.motion.x;
		Mouse.y = e.motion.y;

		ev.x = Mouse.x;
		ev.y = Mouse.y;
		Mouse.lButton = true;

		fireEvent(Event::MOUSEDOWN, ev);
	}
		break;
	case SDL_MOUSEBUTTONUP:
	{
		MouseEvent ev;
		Mouse.x = e.motion.x;
		Mouse.y = e.motion.y;

		ev.x = Mouse.x;
		ev.y = Mouse.y;
		Mouse.lButton = false;

		fireEvent(Event::MOUSECLICK, ev);
	}
		break;
	}
}

void glWindow::gameLoop(void(*update)(long dT), void (*glRender)()) {
	SDL_Event e;

	uint32_t timeStart = SDL_GetTicks();
	uint32_t timeLast = timeStart;

	dontQuit = true;

	while (dontQuit) {
		while (SDL_PollEvent(&e))
			handleEvent(e);

		timeLast = timeStart;
		timeStart = SDL_GetTicks();

		std::sort(Entity::entities.begin(), Entity::entities.end(), [](Entity* a, Entity* b) {return a->zIndex > b->zIndex; });

		update(timeStart - timeLast);
		for (Entity* e : Entity::entities)
			e->update(this, timeStart - timeLast);

		glUseProgram(shaderID);

		updateCamera();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glRender();

		for (Entity* e : Entity::entities)
			e->draw(this);

		glDisable(GL_BLEND);

		swapBuffers();
	}
}

void glWindow::drawRectTex(GLuint texture, int x1, int y1, int x2, int y2, double texU1, double texV1, double texU2, double texV2) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBegin(GL_QUADS);
	glTexCoord2d(texU1, 1.0 - texV1);
	glVertex2d(x1, y1);
	glTexCoord2d(texU2, 1.0 - texV1);
	glVertex2d(x2, y1);
	glTexCoord2d(texU2, 1.0 - texV2);
	glVertex2d(x2, y2);
	glTexCoord2d(texU1, 1.0 - texV2);
	glVertex2d(x1, y2);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void glWindow::drawRectTex(GLuint texture, int x1, int y1, int x2, int y2, glm::vec4 color, double texU1, double texV1, double texU2, double texV2) {
	glPushAttrib(GL_CURRENT_BIT);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBegin(GL_QUADS);
	glColor4d(color.r, color.g, color.b, color.a);
	glTexCoord2d(texU1, 1.0 - texV1);
	glVertex2d(x1, y1);
	glTexCoord2d(texU2, 1.0 - texV1);
	glVertex2d(x2, y1);
	glTexCoord2d(texU2, 1.0 - texV2);
	glVertex2d(x2, y2);
	glTexCoord2d(texU1, 1.0 - texV2);
	glVertex2d(x1, y2);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	glPopAttrib();
}

void glWindow::pix2Pos(int x, int y, double *xo, double *yo) {
	*xo = (double)x / (double)winWidth;
	*yo = (double)y / (double)winHeight;
}

void glWindow::drawRectTex(GLuint texture, int x1, int y1, int x2, int y2, glm::vec4 color1, glm::vec4 color2, double texU1, double texV1, double texU2, double texV2) {
	glPushAttrib(GL_CURRENT_BIT);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBegin(GL_QUADS);
	glColor4d(color1.r, color1.g, color1.b, color1.a);
	glTexCoord2d(texU1, 1.0 - texV1);
	glVertex2i(x1, y1);
	glTexCoord2d(texU2, 1.0 - texV1);
	glVertex2i(x2, y1);
	glColor4d(color2.r, color2.g, color2.b, color2.a);
	glTexCoord2d(texU2, 1.0 - texV2);
	glVertex2i(x2, y2);
	glTexCoord2d(texU1, 1.0 - texV2);
	glVertex2i(x1, y2);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	glPopAttrib();
}

void glWindow::projection_perspective() {
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glScaled(1, 1, 1);
	//Origin at 0,0
	glTranslated(0, 0, 0);
}

void glWindow::projection_ortho() {
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0, winWidth, winHeight, 0);
	glLoadIdentity();
	//Invert y-axis and double
	glScaled(2.0, -2.0, 1.0);
	//Origin at 0,0
	glTranslated(-0.5, -0.5, 0);
	glScaled(1.0 / (double)winWidth, 1.0 / (double)winHeight, 1.0);
}
