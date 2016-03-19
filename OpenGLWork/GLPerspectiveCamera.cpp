#include "GLPerspectiveCamera.h"
#include "GLShaderPipeline.h"
#include "glm\gtc\quaternion.hpp"
#include <glm\gtc\matrix_transform.hpp>
#include "GLShaderPipeline.h"

GLPerspectiveCamera::GLPerspectiveCamera(GLfloat _fov, GLfloat _zNear, GLfloat _zFar, GLfloat _aspect)
	: GlutRenderable(), config(_fov, _zNear, _zFar, _aspect)
{
}

GLPerspectiveCamera::~GLPerspectiveCamera()
{
}

glm::mat4 GLPerspectiveCamera::GetViewProjectionMatrix()
{
	//glm::vec3 forward = transform.Orientation();
	//glm::vec3 target(0, 0, 0);
	//glm::vec3 up = glm::cross(glm::vec3(1, 0, 0), glm::vec3(forward.x, forward.y, forward.z));

	//glm::mat4 view(glm::lookAt(transform.Position(), target, up));
	//glm::mat4 proj(glm::perspective(config.fov, config.aspect, config.zNear, config.zFar));
	glm::vec3 gCameraPos(transform.Position());
	glm::vec3 target(0, 0, 0);
	glm::vec3 up(0, -1, 0);
	glm::mat4 View = glm::lookAt(
		gCameraPos, // Camera is at (4,3,3), in World Space
		target, // and looks at the origin
		up  // Head is up (set to 0,-1,0 to look upside-down
		);

	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 Projection = glm::perspective(60.0f, 640.0f/480.0f, 0.3f, 1000.0f);
	return Projection * View ;
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
