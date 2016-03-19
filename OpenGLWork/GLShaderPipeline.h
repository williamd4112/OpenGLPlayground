#pragma once

#include <GL\glew.h>
#include <GL\freeglut.h>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <stack>
#include <vector>

#include "GlutRenderable.h"

class GLShaderPipeline
{
public:
	~GLShaderPipeline();

	static void InitWithShaderProgram(GLuint shaderProgram);
	static void SetModelViewMatrix(glm::mat4 mat);
	static void SetViewMatrix(glm::mat4 mat);
	static void SetProjectionMatrix(glm::mat4 mat);
	static void SetTextureUnit(GLuint unit);
	static void GLShaderPipeline::SetWVPMatrix(glm::mat4 & wvp);
	static void GLShaderPipeline::SetWMatrix(glm::mat4 & w);
private:
	static GLuint m_WVPLocation;
	static GLuint m_WorldMatrixLocation;
	static GLuint m_samplerLocation;

	static glm::mat4 worldMatrix;
	static glm::mat4 viewMatrix;
	static glm::mat4 projectionMatrix;

	GLShaderPipeline();
};

