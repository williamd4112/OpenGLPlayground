#pragma once

#include <iostream>
#include <vector>
#include <glm\glm.hpp>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>       // Output data structure
#include <assimp/postprocess.h> // Post processing flags

#include "GLData.hpp"
#include "ogldev_texture.h"

#define INVALD 0xffffffff

struct Vertex
{
	glm::vec3 pos;
	glm::vec2 uv;
	glm::vec3 normal;

	Vertex(const glm::vec3 _pos, const glm::vec3 _normal, const glm::vec2 _uv)
		: pos(_pos), normal(_normal), uv(_uv)
	{

	}
};


struct VertexGroup
{
	GLuint vertexBufferObj;
	GLuint indexBufferObj;
	unsigned int materialIndex;
	unsigned int indexNum;

	VertexGroup() : 
		vertexBufferObj(INVALD), 
		indexBufferObj(INVALD),
		materialIndex(INVALD),
		indexNum(0){}
	~VertexGroup(){}

	void Load(const std::vector<Vertex> &vertexs, const std::vector<unsigned int> &indexs);
};

class GLMeshObject
{
public:
	GLMeshObject();
	~GLMeshObject();

	bool Load(const char *filepath);
	void Render();
private:
	std::vector<VertexGroup> vertexGroups;
	std::vector<Texture*> materials;

	bool LoadMaterial(const aiScene* pScene, const char *filepath);
};

