#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

#define _USE_MATH_DEFINES // for C
#include <math.h>

#include <GL\glew.h>
#include <GL\freeglut.h>
#include <assimp\Importer.hpp>      // C++ importer interface
#include <assimp\scene.h>       // Output data structure
#include <assimp\postprocess.h> // Post processing flags

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\quaternion.hpp>

#include "SOIL.h"

#include "GLTextureFactory.h"

using namespace std;

#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600
#define ToRadian(x) (float)(((x) * M_PI / 180.0f))
#define ToDegree(x) (float)(((x) * 180.0f / M_PI))
#define INVALID 0xffffffff

typedef GLint GLWindowID;

std::string GetDirectoryPath(const std::string &Filename)
{
	string::size_type SlashIndex = Filename.find_last_of("/");
	string Dir;

	if (SlashIndex == string::npos) {
		Dir = ".";
	}
	else if (SlashIndex == 0) {
		Dir = "/";
	}
	else {
		Dir = Filename.substr(0, SlashIndex);
	}
	return Dir;
}

struct MousePos
{
	GLuint x, y;
};

struct Vertex
{
	glm::vec3 pos;
	glm::vec2 uv;
	glm::vec3 normal;

	Vertex(const glm::vec3 _pos, const glm::vec2 _uv, const glm::vec3 _normal = glm::vec3(0))
		: pos(_pos), uv(_uv), normal(_normal)
	{

	}

	Vertex() : normal(glm::vec3(0)){}
	~Vertex(){}
};

struct Texture
{
	Texture(GLenum _target, const char *filepath)
		: target(_target)
	{
		id = SOIL_load_OGL_texture
			(
				filepath,
				SOIL_LOAD_AUTO,
				SOIL_CREATE_NEW_ID,
				SOIL_FLAG_MIPMAPS | SOIL_FLAG_COMPRESS_TO_DXT
				);
	}
	Texture(){}
	~Texture(){}

	bool Load(GLenum _target, const char *filepath)
	{
		target = _target;
		id = SOIL_load_OGL_texture
			(
				filepath,
				SOIL_LOAD_AUTO,
				SOIL_CREATE_NEW_ID,
				SOIL_FLAG_MIPMAPS | SOIL_FLAG_COMPRESS_TO_DXT
				);
		return id > 0;
	}

	void BindTexture(GLenum _unit)
	{
		glActiveTexture(_unit);
		glBindTexture(target, id);
	}

private:
	GLenum target;
	GLuint id;
};

struct Material
{
	static const GLenum Units[8];

	Material()
	{}

	~Material()
	{}

	bool AddTexture(GLenum target, const char *filepath)
	{
		Texture *texture = new Texture;
		if (!texture->Load(target, filepath))
			return false;

		textures.push_back(texture);
		return true;
	}

	void Bind()
	{
		for (int i = 0; i < textures.size(); i++)
		{
			textures[i]->BindTexture(GL_TEXTURE0 + i);
		}
	}
private:
	std::vector<Texture*> textures;
};

struct Mesh
{
	int baseIndex;
	int baseVertex;
	int materialIndex;
	int indexNum;

	Mesh() : 
		materialIndex(INVALID),
		indexNum(0),
		baseIndex(INVALID),
		baseVertex(INVALID){}
	~Mesh() {}
};

struct MeshGroup
{
	MeshGroup() : m_Buffers{ 0 } {}
	~MeshGroup(){}

	bool Load(std::string filepath)
	{
		if (m_Buffers[0] != 0) {
			glDeleteBuffers(4, m_Buffers);
		}

		if (vao != 0) {
			glDeleteVertexArrays(1, &vao);
			vao = 0;
		}

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glGenBuffers(sizeof(m_Buffers) / sizeof(GLuint), m_Buffers);

		Assimp::Importer importer;
		const aiScene *scene = importer.ReadFile(filepath,
			aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_FindDegenerates);
		bool result = false;
		if (scene)
		{
			GLenum err = LoadScene(scene, filepath);
			if (err != GL_NO_ERROR) 
			{
				result = false;
				std::cerr << "Mesh load error " << glewGetErrorString(err) << std::endl;
			}
		}
		else
		{
			std::cerr << "Mesh import error " << filepath << std::endl;
		}

		glBindVertexArray(0);

		return result;
	}

	void Render()
	{
		glBindVertexArray(vao);

		for (unsigned int i = 0; i < meshs.size(); i++) {
			const unsigned int MaterialIndex = meshs[i].materialIndex;

			assert(MaterialIndex < materials.size());

			if (MaterialIndex < meshs.size()) {
				materials[MaterialIndex].Bind();
			}
			glDrawElementsBaseVertex(GL_TRIANGLES,
				meshs[i].indexNum,
				GL_UNSIGNED_INT,
				(void*)(sizeof(unsigned int) * meshs[i].baseIndex),
				meshs[i].baseVertex);
		}

		// Make sure the VAO is not changed from the outside    
		glBindVertexArray(0);
	}
private:

#define INDEX_BUFFER 0    
#define POS_VB       1
#define NORMAL_VB    2
#define TEXCOORD_VB  3    
#define POSITION_LOCATION 0
#define TEX_COORD_LOCATION 1
#define NORMAL_LOCATION 2

	std::vector<Mesh> meshs;
	std::vector<Material> materials;
	GLuint vao;
	GLuint m_Buffers[4];

	GLenum LoadScene(const aiScene *scene, const std::string &filepath)
	{
		meshs.resize(scene->mNumMeshes);
		materials.resize(scene->mNumMaterials);

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> texcoords;
		std::vector<glm::vec3> normals;
		std::vector<unsigned int> indices;

		unsigned int vertexNum = 0;
		unsigned int indexNum = 0;

		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			const aiMesh *mesh = scene->mMeshes[i];
			meshs[i].baseVertex = vertexNum;
			meshs[i].baseIndex = indexNum;
			meshs[i].materialIndex = mesh->mMaterialIndex;
			meshs[i].indexNum = mesh->mNumFaces * 3;

			vertexNum += mesh->mNumVertices;
			indexNum += meshs[i].indexNum;
		}

		positions.reserve(vertexNum);
		texcoords.reserve(vertexNum);
		normals.reserve(vertexNum);
		indices.reserve(indexNum);

		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			const aiMesh *mesh = scene->mMeshes[i];
			if (!LoadMesh(mesh, positions, texcoords, normals, indices))
				return false;
		}

		if (!LoadMaterial(scene, filepath))
			return false;

		glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(positions[0]) * positions.size(), &positions[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(POSITION_LOCATION);
		glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords[0]) * texcoords.size(), &texcoords[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(TEX_COORD_LOCATION);
		glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(normals[0]) * normals.size(), &normals[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(NORMAL_LOCATION);
		glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

		return glGetError();
	}

	bool LoadMesh(const aiMesh *mesh, 
		std::vector<glm::vec3> &positions,
		std::vector<glm::vec2> &texcoords,
		std::vector<glm::vec3> &normals,
		std::vector<unsigned int> &indices)
	{
		const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
		for (int i = 0; i < mesh->mNumVertices; i++)
		{
			const aiVector3D *pos = &mesh->mVertices[i];
			const aiVector3D* texcoord = mesh->HasTextureCoords(0) ? 
				&(mesh->mTextureCoords[0][i]) : &Zero3D;
			const aiVector3D *normal = &mesh->mNormals[i];
			positions.push_back(glm::vec3(pos->x, pos->y, pos->z));
			texcoords.push_back(glm::vec2(texcoord->x, texcoord->y));
			normals.push_back(glm::vec3(normal->x, normal->y, normal->z));
		}
		
		for (int i = 0; i < mesh->mNumFaces; i++)
		{
			const aiFace &face = mesh->mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		return true;
	}

	bool LoadMaterial(const aiScene *scene, const std::string &filepath)
	{
		bool ret = true;
		std::string dir = GetDirectoryPath(filepath);
		for (int i = 0; i < scene->mNumMaterials; i++)
		{
			const aiMaterial *material = scene->mMaterials[i];
			for (int j = 0; j < material->GetTextureCount(aiTextureType_DIFFUSE); j++)
			{
				aiString path;
				if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path,
					NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
				{
					string p(path.data);

					if (p.substr(0, 2) == ".\\") {
						p = p.substr(2, p.size() - 2);
					}

					string FullPath = dir + "/" + p;

					bool result = materials[i].AddTexture(GL_TEXTURE_2D, FullPath.c_str());

					if (!result){
						printf("Error loading texture '%s'\n", FullPath.c_str());
						ret = false;
					}
					else {
						printf("Loaded texture '%s'\n", FullPath.c_str());
					}
				}
			}
		}
		return ret;
	}
};

struct DirectionalLight
{
	glm::vec3 Color;
	float AmbientIntensity;
	glm::vec3 Direction;
	float DiffuseIntensity;
};

std::string ParseShaderSourceFile(const char *filepath)
{
	std::ifstream ifs(filepath);
	std::string contents((std::istreambuf_iterator<char>(ifs)),
		std::istreambuf_iterator<char>());
	return contents;
}

GLuint LoadShader(GLenum shaderType, const char *filepath)
{
	GLuint shaderObj = glCreateShader(shaderType);

	std::string str = ParseShaderSourceFile(filepath);
	const char *vShaderText = str.c_str();
	GLint vShaderTextLen = str.length();
	glShaderSource(shaderObj, 1, &vShaderText, &vShaderTextLen);
	glCompileShader(shaderObj);

	GLint success;
	glGetShaderiv(shaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];
		glGetShaderInfoLog(shaderObj, sizeof(InfoLog), NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", shaderType, InfoLog);
	}
	return shaderObj;
}

void CalcNormals(const unsigned int* pIndices, unsigned int IndexCount,
	Vertex* pVertices, unsigned int VertexCount)
{
	// Accumulate each triangle normal into each of the triangle vertices
	for (unsigned int i = 0; i < IndexCount; i += 3) {
		unsigned int Index0 = pIndices[i];
		unsigned int Index1 = pIndices[i + 1];
		unsigned int Index2 = pIndices[i + 2];
		glm::vec3 v1 = pVertices[Index1].pos - pVertices[Index0].pos;
		glm::vec3 v2 = pVertices[Index2].pos - pVertices[Index0].pos;
		glm::vec3 Normal = glm::cross(v2, v1);
		Normal = glm::normalize(Normal);

		pVertices[Index0].normal += Normal;
		pVertices[Index1].normal += Normal;
		pVertices[Index2].normal += Normal;
	}

	// Normalize all the vertex normals
	for (unsigned int i = 0; i < VertexCount; i++) {
		pVertices[i].normal = glm::normalize(pVertices[i].normal);
	}
}

static void Display();
static void Timer(int t);
static void Reshape(int w, int h);
static void Keyboard(unsigned char key, int x, int y);
static void Special(int key, int x, int y);
static void Mouse(int button, int state, int x, int y);
static void MouseWheel(int wheel, int direction, int x, int y);
static void MouseMotion(int x, int y);
static void Init(const char *name, int x, int y, int w, int h);

//GLCamera gCamera(Vec3(0.0, 0.0, 0.0), Position(0.0f, 0.0f, 20.0f),
//	60, (DEFAULT_WIDTH / DEFAULT_HEIGHT), 0.3, 1125.0);

MousePos lastPos{ 0, 0 };
glm::vec3 gCameraPos(0, 0, 200);

GLuint vao, vbo, ibo;
GLuint textureObj;
GLuint gScaleLocation;
GLuint gWorldLocation;
GLuint gWVP;
GLuint gdirLightColorLocation, gdirLightAmbientIntensityLocation;
GLuint gdirLightDirectionLocation, gdirLightDiffuseIntensityLocation;
GLuint gSampler;
GLuint gEyeWorldPosLocation;
GLuint gMatSpecularIntensityLocation;
GLuint gSpecularPowerLocation;

DirectionalLight dirLight{ glm::vec3(0, 0, 1), 0.8f };
MeshGroup meshGroup;

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);

	Init("Robot", 0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT);

	glewExperimental = true;
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		fprintf(stderr, "main(): glew init error (error: %s)\n",
			glewGetErrorString(err));
		system("pause");
		return -1;
	}

	GLuint gShaderProgram = glCreateProgram();
	GLuint vShader = LoadShader(GL_VERTEX_SHADER, "shader.vs");
	GLuint fShader = LoadShader(GL_FRAGMENT_SHADER, "shader.fs");
	glAttachShader(gShaderProgram, vShader);
	glAttachShader(gShaderProgram, fShader);

	GLint success;

	glLinkProgram(gShaderProgram);
	glGetProgramiv(gShaderProgram, GL_LINK_STATUS, &success);
	if (success == 0) {
		GLchar ErrorLog[1024];
		glGetProgramInfoLog(gShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
	}
	glValidateProgram(gShaderProgram);
	glUseProgram(gShaderProgram);
	glDetachShader(gShaderProgram, vShader);
	glDetachShader(gShaderProgram, fShader);

	//static const float FieldDepth = 20.0f;
	//static const float FieldWidth = 10.0f;

	//Vertex Vertices[4] = { 
	//	Vertex(glm::vec3(-1.0f, -1.0f, 0.5773f), glm::vec2(0.0f, 0.0f)),
	//	Vertex(glm::vec3(0.0f, -1.0f, -1.15475f), glm::vec2(0.5f, 0.0f)),
	//	Vertex(glm::vec3(1.0f, -1.0f, 0.5773f),  glm::vec2(1.0f, 0.0f)),
	//	Vertex(glm::vec3(0.0f, 1.0f, 0.0f),      glm::vec2(0.5f, 1.0f)) };

	//unsigned int Indices[] = { 0, 3, 1,
	//	1, 3, 2,
	//	2, 3, 0,
	//	1, 2, 0 };

	//CalcNormals(Indices, 12, Vertices, 4);

	meshGroup.Load("resource/boblampclean.md5mesh");
	
	gWVP = glGetUniformLocation(gShaderProgram, "gWVP");
	gWorldLocation = glGetUniformLocation(gShaderProgram, "gWorld");
	gSampler = glGetUniformLocation(gShaderProgram, "gSampler");
	gdirLightColorLocation = glGetUniformLocation(gShaderProgram, "gDirectionalLight.Color");
	gdirLightAmbientIntensityLocation = glGetUniformLocation(gShaderProgram, "gDirectionalLight.AmbientIntensity");
	gdirLightDirectionLocation = glGetUniformLocation(gShaderProgram, "gDirectionalLight.Direction");
	gdirLightDiffuseIntensityLocation = glGetUniformLocation(gShaderProgram, "gDirectionalLight.DiffuseIntensity");
	gEyeWorldPosLocation = glGetUniformLocation(gShaderProgram, "gEyeWorldPos");
	gMatSpecularIntensityLocation = glGetUniformLocation(gShaderProgram, "gMatSpecularIntensity");
	gSpecularPowerLocation = glGetUniformLocation(gShaderProgram, "gSpecularPower");
	glUniform1i(gSampler, 0);
	glUniform3f(gdirLightColorLocation, 1, 1, 1);
	glUniform1f(gdirLightAmbientIntensityLocation, 0.55f);
	glUniform3f(gdirLightDirectionLocation, 0, 0, 1);
	glUniform1f(gdirLightDiffuseIntensityLocation, .8f);
	glUniform3f(gEyeWorldPosLocation, gCameraPos.x, gCameraPos.y, gCameraPos.z);
	glUniform1f(gMatSpecularIntensityLocation, 1.0);
	glUniform1f(gSpecularPowerLocation, 8);
	glutMainLoop();

	return 0;
}

static void Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);
	
	meshGroup.Render();
	
	//glBindVertexArray(vao);
	//glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
	//glBindVertexArray(0);

	glDisable(GL_DEPTH_TEST);
	glutSwapBuffers();
}

static void Timer(int t)
{
	float angle = glutGet(GLUT_ELAPSED_TIME) * 0.001f;

	glm::vec3 target(0, 0, 0);
	glm::vec3 look = target - gCameraPos;
	glm::vec3 up = glm::cross(look, glm::vec3(1, 0, 0));
	glm::mat4 View = glm::lookAt(
		gCameraPos, // Camera is at (4,3,3), in World Space
		target, // and looks at the origin
		up  // Head is up (set to 0,-1,0 to look upside-down
	);

	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), ((float)DEFAULT_WIDTH / DEFAULT_HEIGHT), 0.1f, 1000.0f);
	glm::mat4 mvp = Projection * View * Model; 

	glm::vec3 eulerAngle(glm::radians(90.0f), glm::radians(angle * 10.0f), glm::radians(0.0f));
	glm::mat4 translate = glm::translate(glm::mat4(1.0), glm::vec3(0, 50, 0 * (std::cos(angle))));
	glm::mat4 rotate = glm::mat4_cast(glm::quat(eulerAngle));
	glm::mat4 transform = translate * rotate;

	glm::mat4 world = transform;
	glm::mat4 wvp = mvp * world;

	world = glm::transpose(world);
	wvp = glm::transpose(wvp);

	glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, &world[0][0]);
	glUniformMatrix4fv(gWVP, 1, GL_TRUE, &wvp[0][0]);
	glutPostRedisplay();
	glutTimerFunc(1 / 30, Timer, 1);
}

static void Display_Fixed()
{
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glColorMaterial(GL_FRONT, GL_DIFFUSE);
	//glShadeModel(GL_SMOOTH);
	//glEnable(GL_COLOR_MATERIAL);
	//glEnable(GL_TEXTURE_2D);
	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
	//glEnable(GL_DEPTH_TEST);

	//glLightfv(GL_LIGHT0, GL_AMBIENT, LIGHT_AMBIENT);
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, LIGHT_DIFFUSE);
	//glLightfv(GL_LIGHT0, GL_SPECULAR, LIGHT_SPECULAR);
	//glLightfv(GL_LIGHT0, GL_POSITION, LIGHT_DIRECT);	

	//glPushMatrix();
	//	gCamera.Setup();
	//	glMatrixMode(GL_MODELVIEW);
	//	glLoadIdentity();
	//
	//glPopMatrix();
	//glDisable(GL_COLOR_MATERIAL);
	//glDisable(GL_TEXTURE_2D);
	//glDisable(GL_LIGHTING);
	//glDisable(GL_LIGHT0);
	//glDisable(GL_DEPTH_TEST);
	//glutSwapBuffers();
}

static void Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	//gCamera.aspect = w / h;
}

static void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'A':case'a':
		break;
	case 'E':case'e':
		break;
	case 'Q':case'q':
		break;
	case 'D':case'd':
		break;
	case 'W':case'w':
		break;
	case 'S':case's':
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

static void Special(int key, int x, int y)
{
	//switch (key)
	//{
	//case GLUT_KEY_UP:
	//	gCamera.Translate(Vec3(0, 1, 0));
	//	break;
	//case GLUT_KEY_DOWN:
	//	gCamera.Translate(Vec3(0, -1, 0));
	//	break;
	//case GLUT_KEY_LEFT:
	//	gCamera.Translate(Vec3(-1, 0, 0));
	//	break;
	//case GLUT_KEY_RIGHT:
	//	gCamera.Translate(Vec3(1, 0, 0));
	//	break;
	//default:
	//	break;
	//}
}

static void Mouse(int button, int state, int x, int y)
{
	lastPos.x = x;
	lastPos.y = y;
}

static void MouseWheel(int wheel, int direction, int x, int y)
{
	if (direction < 0)
		gCameraPos += glm::vec3(0, 0, 10);
	else if (direction > 0)
		gCameraPos -= glm::vec3(0, 0, 10);
	glutPostRedisplay();
}

static void MouseMotion(int x, int y)
{

}

static void Init(const char *name, int x, int y, int w, int h)
{
	glutInitWindowPosition(x, y);
	glutInitWindowSize(w, h);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glClearColor(0, 0, 0, 1);
	glutCreateWindow(name);
	glutDisplayFunc(Display);
	glutTimerFunc(1/30, Timer, 1);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(Special);
	glutMouseFunc(Mouse);
	glutMouseWheelFunc(MouseWheel);
	glutMotionFunc(MouseMotion);
}