#include "glUtils.h"
#include "stdafx.h"

Cube::Cube(float x, float y, float z) {
	Model = glm::mat4(1.0f);

	Model[3][0] = x;
	Model[3][1] = y;
	Model[3][2] = z;

	if (c.CubeMatrixID == NULL)
		c.CubeMatrixID = glGetUniformLocation(programID, "MVP");

	if (c.Cubevertexbuffer == NULL) {
		glGenBuffers(1, &c.Cubevertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, c.Cubevertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
	}

	if (c.Cubecolorbuffer == NULL) {
		glGenBuffers(1, &c.Cubecolorbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, c.Cubecolorbuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
	}
}


void Cube::translate(float x, float y, float z) {
	Model[3][0] = x;
	Model[3][1] = y;
	Model[3][2] = z;
}

void Cube::shift(float dx, float dy, float dz) {
	Model[3][0] += dx;
	Model[3][1] += dy;
	Model[3][2] += dz;
}

void Cube::draw() {
	mvp = /*Transform * */Model;
	glUniformMatrix4fv(c.CubeMatrixID, 1, GL_FALSE, &mvp[0][0]);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, c.Cubevertexbuffer);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, c.Cubecolorbuffer);
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glDrawArrays(GL_TRIANGLES, 0, 12 * 3);

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
}


void objModel::glSetUniform(const char *uniformName, int value) {
	glUniform1i(glGetUniformLocation(glTargetWin->shaderID, uniformName), value);
}
void objModel::glSetUniform(const char *uniformName, glm::vec3 value) {
	glUniform3f(glGetUniformLocation(glTargetWin->shaderID, uniformName), value.x, value.y, value.z);
}
void objModel::glSetUniform(const char *uniformName, glm::vec4 value) {
	glUniform4f(glGetUniformLocation(glTargetWin->shaderID, uniformName), value.x, value.y, value.z, value.w);
}
void objModel::glSetUniform(const char *uniformName, float value) {
	glUniform1f(glGetUniformLocation(glTargetWin->shaderID, uniformName), value);
}

bool objModel::loadObj(const char* filename) {
	struct Pointlist {
		std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	};

	std::vector< glm::vec4 > temp_vertices;
	std::vector< glm::vec3 > temp_uvs;
	std::vector< glm::vec3 > temp_normals;

	Pointlist tri, quad;

	std::ifstream file(filename);
	if (!file.is_open()) return 0;

	std::string line, token;
	glm::vec4 vertex;
	glm::vec3 uv;
	glm::vec3 normal;

	unsigned int vertexIndex[4], uvIndex[4], normalIndex[4];

	while (!file.eof()) {
		std::getline(file, line);
		std::istringstream ss(line);

		ss >> token;

		if (token == "v") {
			ss >> vertex.x;
			ss >> vertex.y;
			ss >> vertex.z;

			vertex.w = (float)(rand()*10.0f / RAND_MAX);
			temp_vertices.push_back(vertex);
		} else if (token == "vt") {
			ss >> uv.x;
			ss >> uv.y;
			temp_uvs.push_back(uv);
		} else if (token == "vn") {
			ss >> normal.x;
			ss >> normal.y;
			ss >> normal.z;
			temp_normals.push_back(normal);
		} else if (token == "f") {
			INT64 spaces = std::count(line.begin(), line.end(), ' ');
			INT64 i;
			//char dummy;
			for (i = 0; i < spaces; i++) {
				ss >> vertexIndex[i];
				//ss >> dummy;
				ss >> uvIndex[i];
				//ss >> dummy;
				ss >> normalIndex[i];
			}

			if (i == 3) {
				tri.vertexIndices.push_back(vertexIndex[0]);
				tri.vertexIndices.push_back(vertexIndex[1]);
				tri.vertexIndices.push_back(vertexIndex[2]);

				tri.uvIndices.push_back(uvIndex[0]);
				tri.uvIndices.push_back(uvIndex[1]);
				tri.uvIndices.push_back(uvIndex[2]);

				tri.normalIndices.push_back(normalIndex[0]);
				tri.normalIndices.push_back(normalIndex[1]);
				tri.normalIndices.push_back(normalIndex[2]);
			} else if (i == 4) {
				quad.vertexIndices.push_back(vertexIndex[0]);
				quad.vertexIndices.push_back(vertexIndex[1]);
				quad.vertexIndices.push_back(vertexIndex[2]);
				quad.vertexIndices.push_back(vertexIndex[3]);

				quad.uvIndices.push_back(uvIndex[0]);
				quad.uvIndices.push_back(uvIndex[1]);
				quad.uvIndices.push_back(uvIndex[2]);
				quad.uvIndices.push_back(uvIndex[3]);

				quad.normalIndices.push_back(normalIndex[0]);
				quad.normalIndices.push_back(normalIndex[1]);
				quad.normalIndices.push_back(normalIndex[2]);
				quad.normalIndices.push_back(normalIndex[3]);
			}
		}
	}

	// For each vertex of each triangle
	for (unsigned int i = 0; i < tri.vertexIndices.size(); i++) {
		unsigned int vtxIdx = tri.vertexIndices[i];
		unsigned int uvIdx = tri.uvIndices[i];
		unsigned int nrmlIdx = tri.normalIndices[i];

		// Get the attributes thanks to the index
		if (vtxIdx < temp_vertices.size()) {
			glm::vec4 vertex = temp_vertices[vtxIdx - 1];
			vertices.push_back(vertex);
		}
		if (uvIdx < temp_uvs.size()) {
			glm::vec3 uv = temp_uvs[uvIdx - 1];
			uvs.push_back(uv);
		}
		if (nrmlIdx < temp_normals.size()) {
			glm::vec3 normal = temp_normals[nrmlIdx - 1];
			normals.push_back(normal);
		}
	}

	quadStart = tri.vertexIndices.size();

	//Quads
	for (unsigned int i = 0; i < quad.vertexIndices.size(); i++) {
		unsigned int vtxIdx = quad.vertexIndices[i];
		unsigned int uvIdx = quad.uvIndices[i];
		unsigned int nrmlIdx = quad.normalIndices[i];

		// Get the attributes thanks to the index
		if (vtxIdx < temp_vertices.size()) {
			glm::vec4 vertex = temp_vertices[vtxIdx - 1];
			vertices.push_back(vertex);
		}
		if (uvIdx < temp_uvs.size()) {
			glm::vec3 uv = temp_uvs[uvIdx - 1];
			uvs.push_back(uv);
		}
		if (nrmlIdx < temp_normals.size()) {
			glm::vec3 normal = temp_normals[nrmlIdx - 1];
			normals.push_back(normal);
		}
	}

	file.close();

	if (vertices.size() > 0) {
		glGenBuffers(1, &vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec4), &vertices[0], GL_STATIC_DRAW);
	}

	if (uvs.size() > 0) {
		glGenBuffers(1, &uvbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
	}

	if (normals.size() > 0) {
		glGenBuffers(1, &normalbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
	}
	return true;
}

int lastIndexOf(const char * s, char target) {
	int ret = -1;
	int curIdx = 0;
	while (s[curIdx] != '\0') {
		if (s[curIdx] == target) ret = curIdx;
		curIdx++;
	}
	return ret;
}

class Vtex {
public:
	glm::vec3 v;
	float weight[3];
	long boneIdx[3], count;
	Vtex() {
		boneIdx[0] = boneIdx[1] = boneIdx[2] = count = 0;
		weight[0] = weight[1] = weight[2] = 0.0f;
	}
};

std::vector<Material> loadedMats;
std::vector<glImage> loadedTextures;
std::vector<objModel::Light*> objModel::lightList;

bool objModel::loadXML(const char* filename) {
	IStream *pFileStream = NULL;
	IXmlReader *pReader = NULL;
	XmlNodeType nodeType;
	const WCHAR* pwszLocalName;

	const WCHAR *x_xml, *y_xml, *z_xml, *skeleton_xml, *matName;

	std::vector< Vtex > temp_vertices;
	std::vector< glm::vec3 > temp_normals;
	std::vector< glm::vec2 > temp_uvs;

	std::vector< long > vertexIndices;
	//std::vector<long> subMeshes;
	long currentSubmesh = 0;
	bool submeshesTag = false;

	SHCreateStreamOnFileA(filename, STGM_READ, &pFileStream);
	if (!pFileStream) return false;
	CreateXmlReader(__uuidof(IXmlReader), (void**)&pReader, NULL);
	pReader->SetProperty(XmlReaderProperty_DtdProcessing, DtdProcessing_Prohibit);
	pReader->SetInput(pFileStream);

	while (S_OK == pReader->Read(&nodeType)) {
		switch (nodeType) {
		case XmlNodeType_Element:
			pReader->GetLocalName(&pwszLocalName, NULL);
			if (StrCmpW(pwszLocalName, L"position") == 0) {
				//x
				pReader->MoveToAttributeByName(L"x", NULL);
				pReader->GetValue(&x_xml, NULL);

				//y
				pReader->MoveToAttributeByName(L"y", NULL);
				pReader->GetValue(&y_xml, NULL);

				//z
				pReader->MoveToAttributeByName(L"z", NULL);
				pReader->GetValue(&z_xml, NULL);

				Vtex vertex;
				vertex.v.x = wcstof(x_xml, NULL);
				vertex.v.y = wcstof(y_xml, NULL);
				vertex.v.z = wcstof(z_xml, NULL);
				temp_vertices.push_back(vertex);
			} else if (StrCmpW(pwszLocalName, L"normal") == 0) {
				//x
				pReader->MoveToAttributeByName(L"x", NULL);
				pReader->GetValue(&x_xml, NULL);

				//y
				pReader->MoveToAttributeByName(L"y", NULL);
				pReader->GetValue(&y_xml, NULL);

				//z
				pReader->MoveToAttributeByName(L"z", NULL);
				pReader->GetValue(&z_xml, NULL);

				glm::vec3 normal;
				normal.x = wcstof(x_xml, NULL);
				normal.y = wcstof(y_xml, NULL);
				normal.z = wcstof(z_xml, NULL);
				temp_normals.push_back(normal);
			} else if (StrCmpW(pwszLocalName, L"texcoord") == 0) {
				//u
				pReader->MoveToAttributeByName(L"u", NULL);
				pReader->GetValue(&x_xml, NULL);

				//v
				pReader->MoveToAttributeByName(L"v", NULL);
				pReader->GetValue(&y_xml, NULL);

				//V is inverted
				glm::vec2 uv;
				uv.x = wcstof(x_xml, NULL);
				uv.y = 1.0f - wcstof(y_xml, NULL);
				temp_uvs.push_back(uv);
			} else if (StrCmpW(pwszLocalName, L"submeshes") == 0) {
				submeshesTag = true;
			} else if (StrCmpW(pwszLocalName, L"submesh") == 0) {
				if (!submeshesTag) continue;
				long id = (-currentSubmesh) - 1;
				vertexIndices.push_back(id);
				currentSubmesh++;
				pReader->MoveToAttributeByName(L"material", NULL);
				pReader->GetValue(&matName, NULL);
				char paramVal[256];
				wcstombs_s(NULL, paramVal, matName, 256);
				Material *mat = nullptr;
				for (std::vector<Material>::iterator i = loadedMats.begin(); i != loadedMats.end(); i++) {
					if (strcmp((*i).matName, paramVal) == 0) {
						mat = &(*i);
						break;
					}
				}

				if (mat == nullptr)
					mat = &DefMaterial;

				subMesh smesh;
				smesh.mat = *mat;

				pReader->MoveToAttributeByName(L"operationtype", NULL);
				pReader->GetValue(&matName, NULL);
				if (StrCmpW(matName, L"triangle_list") == 0)
					smesh.meshKind = GL_TRIANGLES;
				else if (StrCmpW(matName, L"line_list") == 0)
					smesh.meshKind = GL_LINES;
				else
					smesh.meshKind = GL_TRIANGLES;

				submeshList.push_back(smesh);

			} else if (StrCmpW(pwszLocalName, L"face") == 0) {
				//v1
				pReader->MoveToAttributeByName(L"v1", NULL);
				pReader->GetValue(&x_xml, NULL);

				//v2
				pReader->MoveToAttributeByName(L"v2", NULL);
				pReader->GetValue(&y_xml, NULL);

				//v3
				pReader->MoveToAttributeByName(L"v3", NULL);
				pReader->GetValue(&z_xml, NULL);

				long v1, v2, v3;
				v1 = wcstol(x_xml, NULL, 0x0A);
				v2 = wcstol(y_xml, NULL, 0x0A);
				v3 = wcstol(z_xml, NULL, 0x0A);
				//v1.vertIdx = v1.normalIdx = v1.uvIdx = wcstol(x_xml, NULL, 0x0A);
				//v2.vertIdx = v2.normalIdx = v2.uvIdx = wcstol(y_xml, NULL, 0x0A);
				//v3.vertIdx = v3.normalIdx = v3.uvIdx = wcstol(z_xml, NULL, 0x0A);

				vertexIndices.push_back(v1);
				vertexIndices.push_back(v2);
				vertexIndices.push_back(v3);
			} else if (StrCmpW(pwszLocalName, L"skeletonlink") == 0) {
				//name
				pReader->MoveToFirstAttribute();
				pReader->GetValue(&skeleton_xml, NULL);

				//Read Skeleton file
				char skelxml_url[256];
				char skelxml_rel[256];
				int pathIdx = lastIndexOf(filename, '\\');

				strncpy_s(skelxml_rel, filename, pathIdx + 1);
				wcstombs_s(NULL, skelxml_url, skeleton_xml, 256);
				strcat_s(skelxml_url, ".xml");
				strcat_s(skelxml_rel, skelxml_url);
				readSkeleton(skelxml_rel);
			} else if (StrCmpW(pwszLocalName, L"vertexboneassignment") == 0) {
				//Vertex Index
				pReader->MoveToAttributeByName(L"vertexindex", NULL);
				pReader->GetValue(&x_xml, NULL);
				unsigned long vID = wcstol(x_xml, NULL, 0x0A);

				//BoneIndex
				pReader->MoveToAttributeByName(L"boneindex", NULL);
				pReader->GetValue(&y_xml, NULL);
				unsigned long bID = wcstol(y_xml, NULL, 0x0A);

				//Weight
				pReader->MoveToAttributeByName(L"weight", NULL);
				pReader->GetValue(&z_xml, NULL);
				float wht = wcstof(z_xml, NULL);

				if (temp_vertices[vID].count < 3) {
					temp_vertices[vID].boneIdx[temp_vertices[vID].count] = bID;
					temp_vertices[vID].weight[temp_vertices[vID].count] = wht;
					temp_vertices[vID].count++;
				}
			}

			break;

		case XmlNodeType_EndElement:
			pReader->GetLocalName(&pwszLocalName, NULL);
			if (StrCmpW(pwszLocalName, L"submeshes") == 0)
				submeshesTag = false;
			break;
		}
	}

	if (pFileStream) pFileStream->Release();
	if (pReader) pReader->Release();
	pFileStream = NULL;
	pReader = NULL;

	long parseSubmeshId = 0;
	submeshList[0].verticesStart = 0;

	// For each vertex of each triangle
	for (unsigned int i = 0; i < vertexIndices.size(); i++) {
		if (vertexIndices[i] < 0) {
			submeshList[parseSubmeshId].verticesEnd = vertices.size();
			parseSubmeshId = -vertexIndices[i] - 1;
			submeshList[parseSubmeshId].verticesStart = vertices.size();
			continue;
		}
		long vtxIdx = vertexIndices[i];
		long *boneIdx = temp_vertices[vtxIdx].boneIdx;
		float *weightArr = temp_vertices[vtxIdx].weight;

		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[vtxIdx].v;
		glm::vec3 normal = temp_normals[vtxIdx];
		glm::vec3 weight = glm::vec3(weightArr[0], weightArr[1], weightArr[2]);
		glm::ivec3 boneId = glm::ivec3(boneIdx[0], boneIdx[1], boneIdx[2]);
		glm::vec2 uv = temp_uvs.size() > 0 ? temp_uvs[vtxIdx] : glm::vec2(0.0f, 0.0f);

		// Put the attributes in buffers
		vertices.push_back(vertex);
		uvs.push_back(uv);
		normals.push_back(normal);
		weights.push_back(weight);
		boneIds.push_back(boneId);
	}
	submeshList[parseSubmeshId].verticesEnd = vertices.size();

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

	glGenBuffers(1, &weightsbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, weightsbuffer);
	glBufferData(GL_ARRAY_BUFFER, weights.size() * sizeof(glm::vec3), &weights[0], GL_STATIC_DRAW);

	glGenBuffers(1, &boneIdbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, boneIdbuffer);
	glBufferData(GL_ARRAY_BUFFER, boneIds.size() * sizeof(glm::ivec3), &boneIds[0], GL_STATIC_DRAW);

	return true;
}

void objModel::Bone::CalculateBoneToWorldSpaceMatrix(std::vector<Bone> &Bones) {
	glm::mat4 T = glm::translate(pos);
	glm::mat4 R = glm::rotate(angle, rot);
	glm::mat4 localTransform = T * R;

	if (parent >= 0)
		BoneToWorldSpace = Bones[parent].BoneToWorldSpace*localTransform;
	else
		BoneToWorldSpace = localTransform;

	for (std::vector<long>::iterator i = children.begin(); i != children.end(); i++)
		Bones[(*i)].CalculateBoneToWorldSpaceMatrix(Bones);
}

void objModel::readSkeleton(char *filename) {
	IStream *pFileStream = NULL;
	IXmlReader *pReader = NULL;
	XmlNodeType nodeType;
	const WCHAR* pwszLocalName;

	const WCHAR *x_xml, *y_xml, *z_xml, *theta_xml;

	SHCreateStreamOnFileA(filename, STGM_READ, &pFileStream);
	if (!pFileStream) return;
	CreateXmlReader(__uuidof(IXmlReader), (void**)&pReader, NULL);
	pReader->SetProperty(XmlReaderProperty_DtdProcessing, DtdProcessing_Prohibit);
	pReader->SetInput(pFileStream);

	unsigned long boneID;
	char boneName[100];

	bool done = false;

	while (S_OK == pReader->Read(&nodeType) && !done) {
		switch (nodeType) {
		case XmlNodeType_Element:
			pReader->GetLocalName(&pwszLocalName, NULL);
			if (StrCmpW(pwszLocalName, L"bone") == 0) {
				//Name
				pReader->MoveToAttributeByName(L"name", NULL);
				pReader->GetValue(&x_xml, NULL);
				wcstombs_s(NULL, boneName, x_xml, 100);

				//ID
				pReader->MoveToAttributeByName(L"id", NULL);
				pReader->GetValue(&y_xml, NULL);
				boneID = wcstol(y_xml, NULL, 0x0A);

				Bone b;
				strncpy_s(b.name, boneName, 100);
				b.id = boneID;
				bonesList.push_back(b);
			} else if (StrCmpW(pwszLocalName, L"position") == 0) {
				//x
				pReader->MoveToAttributeByName(L"x", NULL);
				pReader->GetValue(&x_xml, NULL);

				//y
				pReader->MoveToAttributeByName(L"y", NULL);
				pReader->GetValue(&y_xml, NULL);

				//z
				pReader->MoveToAttributeByName(L"z", NULL);
				pReader->GetValue(&z_xml, NULL);

				bonesList[boneID].pos = glm::vec3(wcstof(x_xml, NULL), wcstof(y_xml, NULL), wcstof(z_xml, NULL));
			} else if (StrCmpW(pwszLocalName, L"rotation") == 0) {
				//angle
				pReader->MoveToFirstAttribute();
				pReader->GetValue(&theta_xml, NULL);
				bonesList[boneID].angle = wcstof(theta_xml, NULL);
			} else if (StrCmpW(pwszLocalName, L"animations") == 0) {
				done = true;
			} else if (StrCmpW(pwszLocalName, L"axis") == 0) {
				//x
				pReader->MoveToAttributeByName(L"x", NULL);
				pReader->GetValue(&x_xml, NULL);

				//y
				pReader->MoveToAttributeByName(L"y", NULL);
				pReader->GetValue(&y_xml, NULL);

				//z
				pReader->MoveToAttributeByName(L"z", NULL);
				pReader->GetValue(&z_xml, NULL);

				bonesList[boneID].rot = glm::vec3(wcstof(x_xml, NULL), wcstof(y_xml, NULL), wcstof(z_xml, NULL));
			} else if (StrCmpW(pwszLocalName, L"boneparent") == 0) {
				long ChildId, ParentId;

				//bone
				pReader->MoveToAttributeByName(L"bone", NULL);
				pReader->GetValue(&x_xml, NULL);
				wcstombs_s(NULL, boneName, x_xml, 100);
				for (std::vector<Bone>::iterator i = bonesList.begin(); i != bonesList.end(); i++) {
					if (strcmp((*i).name, boneName) == 0) {
						ChildId = (*i).id;
						break;
					}
				}

				//parent
				pReader->MoveToAttributeByName(L"parent", NULL);
				pReader->GetValue(&y_xml, NULL);
				wcstombs_s(NULL, boneName, y_xml, 100);
				for (std::vector<Bone>::iterator i = bonesList.begin(); i != bonesList.end(); i++) {
					if (strcmp((*i).name, boneName) == 0) {
						ParentId = (*i).id;
						break;
					}
				}

				bonesList[ChildId].parent = ParentId;
				bonesList[ParentId].children.push_back(ChildId);
			}

			break;
		}
	}

	if (pFileStream) pFileStream->Release();
	if (pReader) pReader->Release();
	pFileStream = NULL;
	pReader = NULL;

	//Bring bones to the real world
	for (std::vector<Bone>::iterator i = bonesList.begin(); i != bonesList.end(); i++) {
		if ((*i).parent == -1)
			(*i).CalculateBoneToWorldSpaceMatrix(bonesList);
	}

	for (std::vector<Bone>::iterator i = bonesList.begin(); i != bonesList.end(); i++) {
		(*i).inverseBone = glm::inverse((*i).BoneToWorldSpace);
		bones.push_back((*i).inverseBone*(*i).BoneToWorldSpace);
	}
}


void objModel::setPose() {
	for (std::vector<Bone>::iterator i = bonesList.begin(); i != bonesList.end(); i++) {
		if ((*i).parent == -1)
			(*i).CalculateBoneToWorldSpaceMatrix(bonesList);
	}

	unsigned int c = 0;
	for (c = 0; c < bonesList.size(); c++)
		bones[c] = bonesList[c].BoneToWorldSpace * bonesList[c].inverseBone;
}

void objModel::scale(float sx, float sy, float sz) {
	Scale = glm::scale(glm::vec3(sx, sy, sz));
}
void objModel::dscale(float sx, float sy, float sz) {
	Scale = glm::scale(glm::vec3(sx, sy, sz)) * Scale;
}
void objModel::rotate(float sx, float sy, float sz, float theta) {
	Rotation = glm::rotate(theta, glm::vec3(sx, sy, sz));
}
void objModel::drotate(float sx, float sy, float sz, float theta) {
	Rotation = glm::rotate(Rotation, theta, glm::vec3(sx, sy, sz));
}
void objModel::d1rotate(float sx, float sy, float sz, float theta) {
	Rotation = Rotation * glm::rotate(theta, glm::vec3(sx, sy, sz));
}
void objModel::d2rotate(float sx, float sy, float sz, float theta) {
	Rotation = glm::rotate(theta, glm::vec3(sx, sy, sz)) * Rotation;
}
void objModel::translate(float x, float y, float z) {
	Translation = glm::translate(glm::vec3(x, y, z));
}
void objModel::dtranslate(float dx, float dy, float dz) {
	Translation = glm::translate(Translation, glm::vec3(dx, dy, dz));
}

void objModel::setOrigin(float x, float y, float z) {
	origin.x = x;
	origin.y = y;
	origin.z = z;
}

void objModel::draw() {
	Model = glm::translate(origin) * Translation * Rotation * Scale;

	glSetUniform("numLights", (int)objModel::lightList.size());
	char lightUniformName[128];
	for (unsigned int i = 0; i < objModel::lightList.size() && i < MAX_LIGHTS; i++) {
		sprintf_s(lightUniformName, "allLights[%d].position", i);
		glSetUniform(lightUniformName, objModel::lightList[i]->position);
		sprintf_s(lightUniformName, "allLights[%d].intensities", i);
		glSetUniform(lightUniformName, objModel::lightList[i]->intensities);
		sprintf_s(lightUniformName, "allLights[%d].attenuation", i);
		glSetUniform(lightUniformName, objModel::lightList[i]->attenuation);
		sprintf_s(lightUniformName, "allLights[%d].ambientCoefficient", i);
		glSetUniform(lightUniformName, objModel::lightList[i]->ambientCoefficient);
		sprintf_s(lightUniformName, "allLights[%d].coneAngle", i);
		glSetUniform(lightUniformName, objModel::lightList[i]->coneAngle);
		sprintf_s(lightUniformName, "allLights[%d].coneDirection", i);
		glSetUniform(lightUniformName, objModel::lightList[i]->coneDirection);
	}

	if (useBones && bones.size() > 0) {
		glUniform1i(hasBones, 1);
		glUniformMatrix4fv(BonesID, (int)bones.size(), GL_FALSE, &bones[0][0][0]);
	} else glUniform1i(hasBones, 0);
	
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &Model[0][0]);
	glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &(glTargetWin->View[0][0]));
	glUniformMatrix4fv(ProjectionID, 1, GL_FALSE, &(glTargetWin->Projection[0][0]));

	//Pass buffers to Shader
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, weightsbuffer);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(4);
	glBindBuffer(GL_ARRAY_BUFFER, boneIdbuffer);
	glVertexAttribIPointer(4, 3, GL_INT, 0, (void*)0);


	for (std::vector<subMesh>::iterator i = submeshList.begin(); i != submeshList.end(); i++) {
		//Material
		glUniform3f(g_uniformMaterialEmissive, (*i).mat.emissive[0], (*i).mat.emissive[1], (*i).mat.emissive[2]);
		glUniform3f(g_uniformMaterialDiffuse, (*i).mat.diffuse[0], (*i).mat.diffuse[1], (*i).mat.diffuse[2]);
		glUniform3f(g_uniformMaterialSpecular, (*i).mat.specular[0], (*i).mat.specular[1], (*i).mat.specular[2]);
		glUniform3f(g_uniformMaterialAmbient, (*i).mat.ambient[0], (*i).mat.ambient[1], (*i).mat.ambient[2]);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, (*i).mat.Texture);
		glUniform1i(TextureID, 0);
		
		//Draw the vertices
		glDrawArrays((*i).meshKind, (*i).verticesStart, (*i).verticesEnd);
	}

	glDisableVertexAttribArray(4);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
}

objModel::objModel(glWindow *glTarget) {
	Model = glm::mat4(1.0f);
	Translation = glm::mat4(1.0f);
	Scale = glm::mat4(1.0f);
	Rotation = glm::mat4(1.0f);
	origin = glm::vec3(0.0f);

	glTargetWin = glTarget;

	hasBones = glGetUniformLocation(glTargetWin->shaderID, "hasBones");
	BonesID = glGetUniformLocation(glTargetWin->shaderID, "bones");

	TextureID = glGetUniformLocation(glTargetWin->shaderID, "diffuseTexture");
	ProjectionID = glGetUniformLocation(glTargetWin->shaderID, "P");
	ViewMatrixID = glGetUniformLocation(glTargetWin->shaderID, "V");
	ModelMatrixID = glGetUniformLocation(glTargetWin->shaderID, "M");

	//Material properties
	g_uniformMaterialEmissive = glGetUniformLocation(glTargetWin->shaderID, "MaterialEmissiveColor");
	g_uniformMaterialDiffuse = glGetUniformLocation(glTargetWin->shaderID, "MaterialDiffuseColor");
	g_uniformMaterialSpecular = glGetUniformLocation(glTargetWin->shaderID, "MaterialSpecularColor");
	g_uniformMaterialAmbient = glGetUniformLocation(glTargetWin->shaderID, "MaterialAmbientColor");

	//By default, don't use bone model
	useBones = false;
}

objModel::objModel(const char *fileName, glWindow *glTarget) : objModel(glTarget) {
	loadXML(fileName);
	//loadObj(fileName);
}

objModel::~objModel() {
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteBuffers(1, &weightsbuffer);
	glDeleteBuffers(1, &boneIdbuffer);
	vertices.clear();
	normals.clear();
	weights.clear();
	uvs.clear();
	boneIds.clear();
	submeshList.clear();
}

void objModel::exit() {
	std::ofstream out("D:\\t.txt");
	out << Model[3][0] << ", " << Model[3][1] << ", " << Model[3][2];
	out.close();
}

glImage objModel::loadBMP_custom(const char * imagepath) {
	// Data read from the header of the BMP file
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;     // Position in the file where the actual data begins

	glImage img;
	img.imageSize = 0;
	img.width = 0;
	img.height = 0;

	// Open the file
	FILE * file;
	fopen_s(&file, imagepath, "rb");
	if (!file) { printf("Image could not be opened\n"); return img; }

	if (fread(header, 1, 54, file) != 54) { // If not 54 bytes read : problem
		printf("Not a correct BMP file\n");
		return img;
	}

	if (header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file\n");
		return img;
	}

	// Read ints from the byte array
	dataPos = *(int*)&(header[0x0A]);
	img.imageSize = *(int*)&(header[0x22]);
	img.width = *(int*)&(header[0x12]);
	img.height = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (img.imageSize == 0)    img.imageSize = img.width*img.height * 3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0)      dataPos = 54; // The BMP header is done that way

										 // Create a buffer
	img.data = new unsigned char[img.imageSize];

	// Read the actual data from the file into the buffer
	fread(img.data, 1, img.imageSize, file);

	//Everything is in memory now, the file can be closed
	fclose(file);

	return img;
}

glImage objModel::loadImage(const char *imagepath) {
	std::cout << "Try to load texture: " << imagepath << "... ";
	glImage image;
	for (std::vector<glImage>::iterator i = loadedTextures.begin(); i != loadedTextures.end(); i++) {
		if ((*i).imgFile == imagepath) {
			std::cout << "Already loaded!" << std::endl;
			return (*i);
		}
	}

	FIBITMAP *dib(0);
	FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(imagepath, 0);
	if (fif == FIF_UNKNOWN)
		fif = FreeImage_GetFIFFromFilename(imagepath);
	if (fif == FIF_UNKNOWN)
		fif = FIF_PNG;
	if (FreeImage_FIFSupportsReading(fif))
		dib = FreeImage_Load(fif, imagepath, PNG_DEFAULT);
	if (dib == NULL) {
		image.data = COLOR_WHITE_RGB;
		image.width = 1;
		image.height = 1;
		image.imageType = GL_BGR;
		std::cout << "Failed (not found)." << std::endl;
		return image;
	}

	unsigned int imgBPP = FreeImage_GetBPP(dib);

	/*if (imgBPP == 24) {
		FIBITMAP *dib2 = FreeImage_ConvertTo32Bits(dib);
		FreeImage_Unload(dib);
		dib = dib2;
	}*/

	switch (imgBPP) {
	case 24:
		dib = FreeImage_ConvertTo32Bits(dib);
		image.imageType = GL_BGRA;
		break;
	case 32:
		image.imageType = GL_BGRA;
		break;
	default:
		image.imageType = GL_BGR;
	}
	image.width = FreeImage_GetWidth(dib);
	image.height = FreeImage_GetHeight(dib);
	image.imageSize = FreeImage_GetDIBSize(dib);
	image.data = (GLubyte *)malloc(image.imageSize);
	BYTE *bits = FreeImage_GetBits(dib);
	memcpy_s(image.data, image.imageSize, bits, image.imageSize);
	image.imgFile = imagepath;
	FreeImage_Unload(dib);

	std::cout << "Success!" << std::endl;
	loadedTextures.push_back(image);

	return image;
}
GLuint objModel::createTexture(glImage img) {
	// Create one OpenGL texture
	GLuint textureID;
	GLenum error;

	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	//Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width, img.height, 0, img.imageType, GL_UNSIGNED_BYTE, img.data);
	
	while((error = glGetError()) != GL_NO_ERROR)
		std::cout << "glTexImage2D() error: " << error << std::endl;

	//glTexImage2D(GL_TEXTURE_2D, 0, GL_BGR, 1, 1, 0, GL_BGR, GL_UNSIGNED_BYTE, COLOR_WHITE_RGB);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}

void objModel::loadMat(const char * filename) {
	char mat_rel[256];
	int pathIdx = lastIndexOf(filename, '\\');
	strncpy_s(mat_rel, filename, pathIdx + 1);

	std::ifstream material(filename);
	std::string fieldName;
	Material newMat;
	long braces = 0;

	if (material.is_open()) {
		while (!material.eof()) {
			material >> fieldName;
			if (fieldName == "material") {
				newMat = Material();
				material >> newMat.matName;
			} else if (fieldName == "{") {
				braces++;
			} else if (fieldName == "}") {
				if (--braces == 0) loadedMats.push_back(newMat);
			} else if (fieldName == "diffuse") {
				material >> newMat.diffuse[0] >> newMat.diffuse[1] >> newMat.diffuse[2];
			} else if (fieldName == "ambient") {
				material >> newMat.ambient[0] >> newMat.ambient[1] >> newMat.ambient[2];
			} else if (fieldName == "specular") {
				material >> newMat.specular[0] >> newMat.specular[1] >> newMat.specular[2];
			} else if (fieldName == "emissive") {
				material >> newMat.emissive[0] >> newMat.emissive[1] >> newMat.emissive[2];
			} else if (fieldName == "texture") {
				material >> fieldName;
				glImage e = loadImage((mat_rel + fieldName).c_str());
				newMat.Texture = createTexture(e);
			}
		}
		material.close();
	}
}

void objModel::unloadAllMaterials() {
	for (std::vector<Material>::iterator i = loadedMats.begin(); i != loadedMats.end(); i++) {
		glDeleteTextures(1, &(*i).Texture);
	}

	loadedTextures.clear();
	loadedMats.clear();
}
