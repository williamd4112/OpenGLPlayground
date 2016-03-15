#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

#define _USE_MATH_DEFINES // for C
#include <math.h>

#include <GL\glew.h>
#include <GL\freeglut.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\quaternion.hpp>

#include "SOIL.h"

#include "GLTextureFactory.h"
#include "GLData.hpp"

using namespace std;

#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600
#define ToRadian(x) (float)(((x) * M_PI / 180.0f))
#define ToDegree(x) (float)(((x) * 180.0f / M_PI))

typedef GLint GLWindowID;

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

GLCamera gCamera(Vec3(0.0, 0.0, 0.0), Position(0.0f, 0.0f, 20.0f),
	60, (DEFAULT_WIDTH / DEFAULT_HEIGHT), 0.3, 1125.0);

MousePos lastPos{ 0, 0 };

GLuint vbo, ibo;
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

	static const float FieldDepth = 20.0f;
	static const float FieldWidth = 10.0f;

	Vertex Vertices[4] = { 
		Vertex(glm::vec3(-1.0f, -1.0f, 0.5773f), glm::vec2(0.0f, 0.0f)),
		Vertex(glm::vec3(0.0f, -1.0f, -1.15475f), glm::vec2(0.5f, 0.0f)),
		Vertex(glm::vec3(1.0f, -1.0f, 0.5773f),  glm::vec2(1.0f, 0.0f)),
		Vertex(glm::vec3(0.0f, 1.0f, 0.0f),      glm::vec2(0.5f, 1.0f)) };

	unsigned int Indices[] = { 0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		1, 2, 0 };

	CalcNormals(Indices, 12, Vertices, 4);

	textureObj = SOIL_load_OGL_texture
		(
			"resource/test.png",
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_MIPMAPS | SOIL_FLAG_COMPRESS_TO_DXT
			);

	if (!textureObj)
	{
		printf("SOIL loading error: '%s'\n", SOIL_last_result());
	}

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

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
	glUniform1f(gdirLightAmbientIntensityLocation, 0.2f);
	glUniform3f(gdirLightDirectionLocation, 0, 0, 1);
	glUniform1f(gdirLightDiffuseIntensityLocation, .1f);
	glUniform3f(gEyeWorldPosLocation, 5, 5, 5);
	glUniform1f(gMatSpecularIntensityLocation, 1.0);
	glUniform1f(gSpecularPowerLocation, 32);
	glutMainLoop();

	return 0;
}

static void Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)sizeof(glm::vec3));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
	glActiveTexture(GL_TEXTURE0);	
	glBindTexture(GL_TEXTURE_2D, textureObj);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisable(GL_DEPTH_TEST);
	glutSwapBuffers();
}

static void Timer(int t)
{
	float angle = glutGet(GLUT_ELAPSED_TIME) * 0.001f;

	glm::mat4 View = glm::lookAt(
		glm::vec3(5, 5, 5), // Camera is at (4,3,3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down
	);

	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), ((float)DEFAULT_WIDTH / DEFAULT_HEIGHT), 0.1f, 100.0f);
	glm::mat4 mvp = Projection * View * Model; 

	glm::mat4 translate = glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0 * (std::cos(angle))));
	glm::mat4 rotate = glm::rotate(glm::mat4(1.0), angle, glm::vec3(0, 1, 0));
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
	gCamera.aspect = w / h;
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
	switch (key)
	{
	case GLUT_KEY_UP:
		gCamera.Translate(Vec3(0, 1, 0));
		break;
	case GLUT_KEY_DOWN:
		gCamera.Translate(Vec3(0, -1, 0));
		break;
	case GLUT_KEY_LEFT:
		gCamera.Translate(Vec3(-1, 0, 0));
		break;
	case GLUT_KEY_RIGHT:
		gCamera.Translate(Vec3(1, 0, 0));
		break;
	default:
		break;
	}
}

static void Mouse(int button, int state, int x, int y)
{
	lastPos.x = x;
	lastPos.y = y;
}

static void MouseWheel(int wheel, int direction, int x, int y)
{
	if (direction < 0)
		gCamera.Translate(Vec3(0, 0, -1));
	else if (direction > 0)
		gCamera.Translate(Vec3(0, 0, 1));
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