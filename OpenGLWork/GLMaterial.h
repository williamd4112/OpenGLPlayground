#pragma once

#include <vector>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <glm\glm.hpp>
#include <Importer.hpp>      // C++ importer interface
#include <scene.h>       // Output data structure
#include <postprocess.h> // Post processing flags

#define GLMATERIAL_LOAD_ERROR 0xffffffff

class GLMaterial
{
public:
	GLMaterial();
	GLMaterial(const aiMaterial *material, std::string &dir);
	~GLMaterial();

	void Bind(GLenum target);
private:
	std::vector<GLuint> textures;
	glm::vec3 color_diffuse;
	glm::vec3 color_specular;
	glm::vec3 color_ambient;
	glm::vec3 color_emissive;
	glm::float32 shiness;
	glm::float32 shiness_strength;
};

