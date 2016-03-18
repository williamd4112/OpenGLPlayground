#pragma once

#include <glm\glm.hpp>
#include <GL\freeglut.h>

#include "Transform.h"
#include "GlutRenderable.h"

class GLPerspectiveCamera
	: public GlutRenderable
{
public:
	struct PerspectiveConfig
	{
		GLfloat fov;
		GLfloat zNear;
		GLfloat zFar;
		GLfloat aspect;

		PerspectiveConfig(GLfloat fov,
			GLfloat zNear,
			GLfloat zFar,
			GLfloat aspect);
	};

	GLPerspectiveCamera(GLfloat _fov, GLfloat _zNear, GLfloat _zFar, GLfloat _aspect);
	~GLPerspectiveCamera();

	void SetFov(GLfloat fov) { config.fov = fov; }
	void SetZnear(GLfloat zNear) { config.zNear = zNear; }
	void SetZfar(GLfloat zFar) { config.zFar = zFar; }
	void SetAspect(GLfloat aspect) { config.aspect = aspect; }
	Transform &GetTransform() { return transform; }

	void RenderShader();
	void RenderFixedPipeline();
	void FinishRenderFixedPipeline();
private:
	Transform transform;
	PerspectiveConfig config;
};

