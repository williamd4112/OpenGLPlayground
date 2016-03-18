#include "Transform.h"

#include <GL\freeglut.h>

#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\quaternion.hpp"
#include "glm\gtc\type_ptr.hpp"

using namespace glm;

Transform::Transform() :
	position(0), rotation(0), scale(glm::vec3(1, 1, 1))
{

}

Transform::Transform(const glm::vec3 _position = glm::vec3(0), 
	const glm::vec3 _rotation = glm::vec3(0), 
	const glm::vec3 _scale = glm::vec3(1, 1, 1)) :
	position(_position), rotation(_rotation), scale(_scale), transformMatrix(glm::mat4(1.0f))
{

}

Transform::~Transform()
{
}

glm::mat4 Transform::GetTransformMatrix()
{
	return transformMatrix;
}

glm::mat4 Transform::GetInverseTransformMatrix()
{
	return glm::inverse(transformMatrix);
}

void Transform::PushTransformMatrix()
{
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glm::mat4 transpose_transformMatrix((glm::translate(mat4(1.0f), position)));
	glLoadIdentity();
	glMultMatrixf(glm::value_ptr(transformMatrix));
}

void Transform::PopTransformMatrix()
{
	glPopMatrix();
}

void Transform::Translate(const glm::vec3 v)
{
	position += v;
	updateTransformMatrix();
}

void Transform::Rotate(const glm::vec3 eulerAngles)
{
	rotation += eulerAngles;
	updateTransformMatrix();
}

void Transform::Scale(const glm::vec3 v)
{
	scale += v;
	updateTransformMatrix();
}

const glm::vec3 Transform::Orientation()
{
	// Rotation matrix
	glm::quat qPitch = glm::angleAxis(rotation.x, glm::vec3(1, 0, 0));
	glm::quat qYaw = glm::angleAxis(rotation.y, glm::vec3(0, 1, 0));
	glm::quat qRoll = glm::angleAxis(rotation.z, glm::vec3(0, 0, 1));

	glm::quat orientation = qPitch * qYaw * qRoll;
	orientation = glm::normalize(orientation);
	glm::mat4 rotateMatrix = glm::mat4_cast(orientation);
	
	return glm::vec3(rotateMatrix * glm::vec4(0, 0, 1, 0));
}

void Transform::updateTransformMatrix()
{
	// Translation matrix
	glm::mat4 translateMatrix = glm::translate(mat4(1.0f), position);

	// Rotation matrix
	glm::quat qPitch = glm::angleAxis(rotation.x, glm::vec3(1, 0, 0));
	glm::quat qYaw = glm::angleAxis(rotation.y, glm::vec3(0, 1, 0));
	glm::quat qRoll = glm::angleAxis(rotation.z, glm::vec3(0, 0, 1));

	glm::quat orientation = qPitch * qYaw * qRoll;
	orientation = glm::normalize(orientation);
	glm::mat4 rotateMatrix = glm::mat4_cast(orientation);

	// Scale matrix
	glm::mat4 scaleMatrix = glm::scale(mat4(1.0f), scale);

	transformMatrix = translateMatrix  * rotateMatrix * scaleMatrix;
}
