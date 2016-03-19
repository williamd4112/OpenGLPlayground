#pragma once

#include "Transform.h"

class GLObject
{
public:
	GLObject();
	~GLObject();

	Transform &GetTransform() { return transform; }

protected:
	Transform transform;
};

