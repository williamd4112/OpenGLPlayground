#pragma once

#include <vector>
#include "GLMesh.h"
#include "Transform.h"
#include "GlutRenderable.h"
#include "GLObject.h"

class GLMeshObject
	: public GlutRenderable, public GLObject
{
public:
	GLMeshObject();
	~GLMeshObject();
	
	void Load(std::string filepath);
	void RenderShader();
	void RenderFixedPipeline();
	void AddChild(GLMeshObject  *);
	GLMesh::GLMeshEntry &MeshEntry(int i) { return mesh.MeshEntry(i); }
	const GLMesh &Mesh() const { return mesh; }
private:
	GLMesh mesh;
	std::vector<GLMeshObject *> children;
};

