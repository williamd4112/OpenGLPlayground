#include "GLPerspectiveCamera.h"
#include "glm\gtc\quaternion.hpp"

GLPerspectiveCamera::GLPerspectiveCamera(GLfloat _fov, GLfloat _zNear, GLfloat _zFar, GLfloat _aspect)
	: GlutRenderable(), config(_fov, _zNear, _zFar, _aspect)
{
}

GLPerspectiveCamera::~GLPerspectiveCamera()
{
}

void GLPerspectiveCamera::RenderShader()
{
}

void GLPerspectiveCamera::RenderFixedPipeline()
{
	glm::vec3 forward = transform.Orientation();
	glm::vec3 target(
		(transform.Position().x - forward.x),
		(transform.Position().y - forward.y),
		(transform.Position().z - forward.z));
	glm::vec3 up = glm::cross(glm::vec3(forward.x, forward.y, forward.z), glm::vec3(1, 0, 0));

	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(config.fov, config.aspect, config.zNear, config.zFar);
	gluLookAt(transform.Position().x, transform.Position().y, transform.Position().z,
		target.x, target.y, target.z,
		up.x, up.y, up.z);
}

void GLPerspectiveCamera::FinishRenderFixedPipeline()
{
	glPopMatrix();
}

GLPerspectiveCamera::PerspectiveConfig::PerspectiveConfig(GLfloat _fov, GLfloat _zNear, GLfloat _zFar, GLfloat _aspect)
	: fov(_fov), zNear(_zNear), zFar(_zFar), aspect(_aspect)
{
}
