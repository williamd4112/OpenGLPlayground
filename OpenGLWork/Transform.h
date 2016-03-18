#pragma once

#include <glm\glm.hpp>
#include "GlutTransfromable.h"

class Transform
	: public GlutTransfromable
{
public:
	Transform(const glm::vec3 _position, const glm::vec3 _rotation, const glm::vec3 _scale);
	Transform();
	~Transform();

	glm::mat4 GetTransformMatrix();
	glm::mat4 GetInverseTransformMatrix();
	void PushTransformMatrix();
	void PopTransformMatrix();

	void Translate(const glm::vec3 v);
	void Rotate(const glm::vec3 eulerAngles);
	void Scale(const glm::vec3 v);
	const glm::vec3 &Position() { return position; }
	const glm::vec3 &Rotation() { return rotation; }
	const glm::vec3 &Scale() { return scale; }
	const glm::vec3 Orientation();
private:
	glm::vec3 position;
	glm::vec3 rotation; // Euler angle
	glm::vec3 scale;
	glm::mat4 transformMatrix; // Update when Translate, Rotate, Scale

	void updateTransformMatrix();
};

