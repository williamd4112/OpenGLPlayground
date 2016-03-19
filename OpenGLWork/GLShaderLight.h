#pragma once

#include <glm\glm.hpp>
#include <GL\glew.h>
#include <GL\freeglut.h>

class GLShaderLight
{
public:
	GLShaderLight();
	~GLShaderLight();

	void InitWithShaderProgram(GLuint program);
	void SetDirectionalLight(glm::vec4 color, glm::vec3 direction,
		GLfloat ambientIntensity, GLfloat diffuseIntensity);
	void SetEyeWorldPos(const glm::vec3 EyeWorldPos);
	void SetMatSpecularIntensity(float Intensity);
	void SetMatSpecularPower(float Power);
private:
	GLuint m_eyeWorldPosLocation;
	GLuint m_matSpecularIntensityLocation;
	GLuint m_matSpecularPowerLocation;

	struct {
		GLuint Color;
		GLuint AmbientIntensity;
		GLuint Direction;
		GLuint DiffuseIntensity;
	} m_dirLightLocation;
};

