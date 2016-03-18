#pragma once

#include <iostream>
#include <vector>
#include <glm\glm.hpp>

#include <GL\freeglut.h>

#include "GlutRenderable.h"

using namespace glm;

class aiScene;

class Mesh
	: public GlutRenderable
{
public:
#define INVALID_ID 0xffffffff

	struct Vertex
	{
		vec3 pos;
		vec2 texcoord;
		vec3 normal;

		Vertex(const vec3 _pos, const vec2 _texcoord, const vec3 _normal = vec3(0))
			: pos(_pos), texcoord(_texcoord), normal(_normal)
		{

		}

		~Vertex(){}
	};

	struct MeshEntry
	{
		GLuint vbo;
		GLuint ibo;
		
		MeshEntry() : vbo(INVALID_ID), ibo(INVALID_ID)
		{}
		
		~MeshEntry()
		{}
	};

	Mesh();
	~Mesh();

	void Load(std::string filepath);
	void Render();
	void RenderFixedPipeline();
private:
	void LoadScene(const aiScene *scene);
};

