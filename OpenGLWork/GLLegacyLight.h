#pragma once

#include <GL\freeglut.h>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>

class GLLegacyLight
{
public:
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	glm::vec4 position;

	GLLegacyLight(
		const glm::vec4 _ambient, 
		const glm::vec4 _diffuse, 
		const glm::vec4 _specular, 
		const glm::vec4 _position);
	GLLegacyLight();
	~GLLegacyLight();

	void Setup(GLenum lightUnit);
private:
	GLenum lightUnit;
};

