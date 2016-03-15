#pragma once

#include <cstdio>
#include <GL/glew.h>
#include <GL/freeglut.h>

class GLTextureException
{
public:
	enum ExceptionType
	{
		LOAD_INVALID
	};

	GLTextureException(ExceptionType type) : mType(type)
	{}

	ExceptionType GetType()
	{
		return mType;
	}
private:
	ExceptionType mType;
};

class GLTextureFactory
{
public:
	GLTextureFactory();
	~GLTextureFactory();
	
	static GLuint LoadBMP(const char *filepath);
};

