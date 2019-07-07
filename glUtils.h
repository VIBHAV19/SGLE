#pragma once

//Define this somewhere else if you want more/less lights
#ifndef MAX_LIGHTS
#define MAX_LIGHTS 10
#endif

#define GLM_ENABLE_EXPERIMENTAL

#include <vector>
#include <string>
#include <sstream> 
#include <fstream>
#include <iostream>
#include <gl/glew.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <Windows.h>
#include <xmllite.h>
#include <shlwapi.h>
#include <freeimage/FreeImage.h>

class glWindow;

struct Material {
	char matName[256];
	float emissive[3], diffuse[3], specular[3], ambient[3];
	GLuint Texture;
};

struct subMesh {
	Material mat;
	unsigned long verticesStart, verticesEnd;
	DWORD meshKind;
};

static Material DefMaterial = { "Default", {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, 0 };

static UINT8 COLOR_WHITE_RGB[3] = {255, 255, 255};

static const GLfloat g_vertex_buffer_data[] = {
	-1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, 1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f,-1.0f,
	1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f,-1.0f,
	1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f,-1.0f, 1.0f,
	1.0f,-1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f,-1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f,-1.0f,
	-1.0f, 1.0f,-1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f,-1.0f, 1.0f
};
static const GLfloat g_color_buffer_data[] = {
	0.583f,  0.771f,  0.014f,
	0.609f,  0.115f,  0.436f,
	0.327f,  0.483f,  0.844f,
	0.822f,  0.569f,  0.201f,
	0.435f,  0.602f,  0.223f,
	0.310f,  0.747f,  0.185f,
	0.597f,  0.770f,  0.761f,
	0.559f,  0.436f,  0.730f,
	0.359f,  0.583f,  0.152f,
	0.483f,  0.596f,  0.789f,
	0.559f,  0.861f,  0.639f,
	0.195f,  0.548f,  0.859f,
	0.014f,  0.184f,  0.576f,
	0.771f,  0.328f,  0.970f,
	0.406f,  0.615f,  0.116f,
	0.676f,  0.977f,  0.133f,
	0.971f,  0.572f,  0.833f,
	0.140f,  0.616f,  0.489f,
	0.997f,  0.513f,  0.064f,
	0.945f,  0.719f,  0.592f,
	0.543f,  0.021f,  0.978f,
	0.279f,  0.317f,  0.505f,
	0.167f,  0.620f,  0.077f,
	0.347f,  0.857f,  0.137f,
	0.055f,  0.953f,  0.042f,
	0.714f,  0.505f,  0.345f,
	0.783f,  0.290f,  0.734f,
	0.722f,  0.645f,  0.174f,
	0.302f,  0.455f,  0.848f,
	0.225f,  0.587f,  0.040f,
	0.517f,  0.713f,  0.338f,
	0.053f,  0.959f,  0.120f,
	0.393f,  0.621f,  0.362f,
	0.673f,  0.211f,  0.457f,
	0.820f,  0.883f,  0.371f,
	0.982f,  0.099f,  0.879f
};

struct CubeClass {
	GLuint CubeMatrixID = NULL;
	GLuint Cubevertexbuffer = NULL;
	GLuint Cubecolorbuffer = NULL;
};

class Cube {
	glm::mat4 mvp;
	CubeClass c;
	GLuint programID;
public:
	glm::mat4 Model;
	Cube(float, float, float);
	void translate(float, float, float);
	void shift(float, float, float);
	void draw();
};

struct glImage {
	GLubyte* data;
	unsigned int width, height;
	unsigned int imageSize;
	GLuint imageType;
	std::string imgFile;
};

class objModel {
	std::vector < glm::vec3 > vertices;
	std::vector < glm::vec3 > normals;
	std::vector < glm::vec3 > weights;
	std::vector < glm::vec2 > uvs;
	std::vector < glm::ivec3 > boneIds;

	int textNum = 0;
	size_t quadStart;

	glWindow *glTargetWin;

	GLuint vertexbuffer;
	GLuint normalbuffer;
	GLuint uvbuffer;
	GLuint weightsbuffer;
	GLuint boneIdbuffer;
	GLuint ProjectionID;
	GLuint TextureID;
	GLuint ViewMatrixID;
	GLuint ModelMatrixID;
	GLuint hasBones;
	GLuint BonesID;
	GLuint g_uniformMaterialEmissive;
	GLuint g_uniformMaterialDiffuse;
	GLuint g_uniformMaterialSpecular;
	GLuint g_uniformMaterialAmbient;

	void glSetUniform(const char*, int);
	void glSetUniform(const char *, glm::vec3);
	void glSetUniform(const char *, glm::vec4);
	void glSetUniform(const char *, float);
	
public:
	bool loadObj(const char*);
	bool loadXML(const char*);
	void readSkeleton(char*);
	void setPose();
	void scale(float, float, float);
	void dscale(float, float, float);
	void rotate(float, float, float, float);
	void drotate(float, float, float, float);
	void d1rotate(float, float, float, float);
	void d2rotate(float, float, float, float);
	void translate(float, float, float);
	void dtranslate(float, float, float);
	void setOrigin(float x, float y, float z);
	void draw();
	objModel(glWindow*);
	objModel(const char*, glWindow*);
	~objModel();
	void exit();
	
	struct Bone {
		glm::vec3 pos, rot;
		float angle;
		unsigned long id;
		char name[100];
		long parent;
		std::vector<long> children;
		glm::mat4 BoneToWorldSpace;
		glm::mat4 inverseBone;

		Bone() {
			parent = -1;
		}

		void CalculateBoneToWorldSpaceMatrix(std::vector<Bone> &Bones);
	};

	struct Light {
		glm::vec4 position;
		glm::vec3 intensities;
		float attenuation;
		float ambientCoefficient;
		float coneAngle;
		glm::vec3 coneDirection;

		Light() {}

		Light(glm::vec4 pos, glm::vec3 color) {
			position = pos;
			intensities = color;
		}
	};

	bool useBones;
	glm::mat4 Model;
	glm::mat4 Translation;
	glm::mat4 Rotation;
	glm::mat4 Scale;
	glm::vec3 origin;
	std::vector<Bone> bonesList;
	std::vector<glm::mat4> bones;
	std::vector<subMesh> submeshList;
	static std::vector<Light*> lightList;

	static glImage loadBMP_custom(const char*);
	static glImage loadImage(const char*);
	static GLuint createTexture(glImage);
	static void loadMat(const char*);
	static void unloadAllMaterials();
};
