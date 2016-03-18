#pragma once

#include <glm\glm.hpp>

class GlutTransfromable
{
public:
	GlutTransfromable();
	~GlutTransfromable();

	virtual glm::mat4 GetTransformMatrix() = 0;
	virtual glm::mat4 GetInverseTransformMatrix() = 0;
	virtual void PushTransformMatrix() = 0;
	virtual void PopTransformMatrix() = 0;
	
};

