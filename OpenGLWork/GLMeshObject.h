#pragma once

#include "GLMesh.h"
#include "Transform.h"
#include "GlutRenderable.h"

class GLMeshObject
	: public GlutRenderable
{
public:
	GLMeshObject();
	~GLMeshObject();
	
	void Load(std::string filepath);
	void RenderShader();
	void RenderFixedPipeline();
	Transform &GetTransform() { return transform; }
private:
	GLMesh mesh;
	Transform transform;
};

