#pragma once

#include <GL\glew.h>
#include <GL\freeglut.h>
#include <glm\glm.hpp>
#include "GlutRenderable.h"

#define GEMOMERTY_PYRAMID 0

class GLGeometry
	: public GlutRenderable
{
public:
	GLGeometry(const unsigned int type);
	~GLGeometry();
	void RenderShader();
	void RenderFixedpipeline();
private:
	GLuint vbo, ibo;
	unsigned int numVertex, numIndices;
};

