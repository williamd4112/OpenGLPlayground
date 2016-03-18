#pragma once

#include <iostream>
#include <vector>
#include <glm\glm.hpp>

#include "GlutRenderable.h"
#include "GLMaterial.h"

#include <GL\glew.h>
#include <GL\freeglut.h>
#include <Importer.hpp>      // C++ importer interface
#include <scene.h>       // Output data structure
#include <postprocess.h> // Post processing flags



using namespace glm;

class aiScene;

class GLMesh
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

	struct GLMeshEntry
	{
		GLuint vbo;
		GLuint ibo;
		unsigned int numVertexes;
		unsigned int numIndices;
		unsigned int materialIndex;
		
		GLMeshEntry() : 
			vbo(INVALID_ID), 
			ibo(INVALID_ID),
			numVertexes(0),
			numIndices(0),
			materialIndex(0)
		{}
		
		~GLMeshEntry()
		{}
	};

	GLMesh();
	~GLMesh();

	void Load(std::string filepath);
	void RenderShader();
	void RenderFixedPipeline();
private:
	Assimp::Importer importer;
	GLuint vao;
	std::vector<GLMeshEntry> entries;
	std::vector<GLMaterial> materials;

	void LoadScene(const aiScene *scene);
	void LoadMaterial(const aiScene *scene, std::string filepath);
};

