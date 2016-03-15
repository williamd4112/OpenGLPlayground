#pragma once

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <vector>
#include "ImageMagick-6/Magick++.h"

extern GLfloat COLOR_RED[3];
extern GLfloat COLOR_GREEN[3];
extern GLfloat COLOR_BLUE[3];
extern GLfloat COLOR_WHITE[3];
extern GLfloat LIGHT_AMBIENT[4];
extern GLfloat LIGHT_DIFFUSE[4];
extern GLfloat LIGHT_SPECULAR[4];
extern GLfloat LIGHT_DIRECT[4];

struct Vec3
{
	GLfloat x, y, z;
	Vec3() : x(0), y(0), z(0) {}
	Vec3(GLfloat _x, GLfloat _y, GLfloat _z) : x(_x), y(_y), z(_z) {}
	~Vec3() {}

	friend Vec3 operator +(const Vec3 &a, const Vec3 &b)
	{
		return Vec3(a.x + b.x, a.y + b.y, a.z + b.z);
	}

	friend Vec3 operator +=(const Vec3 &a, const Vec3 &b)
	{
		return Vec3(a.x + b.x, a.y + b.y, a.z + b.z);
	}
};

typedef Vec3 Position;
typedef Vec3 Scale;

struct Rotation
{
	GLfloat angle;
	Vec3 axis;

	Rotation() : angle(0), axis(0, 0, 0) {}
	Rotation(GLfloat _angle, GLfloat _x, GLfloat _y, GLfloat _z) :
		angle(_angle), axis(_x, _y, _z) {}
	~Rotation() {}
};

struct Transform
{
	Position position;
	Rotation rotation;
	Scale scale;

	Transform(Position pos) : position(pos) {}
	Transform() : position(0, 0, 0), rotation(), scale(1, 1, 1) {}
	~Transform() {}
};

struct GLObject
{
	Transform transform;
	GLfloat *color;
	GLuint texture;
	std::vector<GLObject*> children;

	GLObject(GLfloat *_color) : color(_color), texture(0) {}
	GLObject() : texture(0) {}
	~GLObject() {}

	virtual void Draw() = 0;

	void Render()
	{
		glPushMatrix();

		glTranslatef(transform.position.x, transform.position.y, transform.position.z);
		glScalef(transform.scale.x, transform.scale.y, transform.scale.z);
		glRotatef(transform.rotation.angle,
			transform.rotation.axis.x, transform.rotation.axis.y, transform.rotation.axis.z);
		
		glColor3fv(color);
		glBindTexture(GL_TEXTURE_2D, texture);
		Draw();
		RenderChildren();
		glPopMatrix();
	}

	void RenderChildren()
	{
		for (std::vector<GLObject*>::iterator it = children.begin();
		it != children.end();
			it++)
		{
			if (*it != NULL)
				(*it)->Render();
		}
	}

	void Attach(GLObject *obj)
	{
		if (obj != NULL)
			children.push_back(obj);
	}
};

struct GLCube :
	GLObject
{
	GLdouble size;

	GLCube(GLdouble _size, GLfloat *_color = COLOR_WHITE) :
		GLObject(_color),
		size(_size)
	{
	}

	~GLCube()
	{}

	void Draw()
	{
		glutSolidCube(size);
	}
};

struct GLSphere
	: GLObject
{
	GLdouble radius;
	GLSphere(GLdouble _radius, GLfloat *_color = COLOR_WHITE) :
		GLObject(_color),
		radius(_radius)
	{
	}
	~GLSphere() {}

	void Draw()
	{
		glutSolidSphere(radius, 100, 100);
	}
};

struct GLCamera
{
	Transform transform;
	Vec3 target;
	GLdouble fov;
	GLdouble aspect;
	GLdouble zNear;
	GLdouble zFar;

	GLCamera(Vec3 target, Position position, GLdouble _fov, GLdouble _aspect, GLdouble _zNear, GLdouble _zFar)
		: target(target), transform(position), fov(_fov), aspect(_aspect), zNear(_zNear), zFar(_zFar)
	{

	}

	~GLCamera() {}

	void Translate(Vec3 v)
	{
		transform.position.x += v.x;
		transform.position.y += v.y;
		transform.position.z += v.z;
	}

	void Rotate(GLfloat angle, Vec3 v)
	{
	}

	void Setup()
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(fov, aspect, zNear, zFar);
		gluLookAt(transform.position.x, transform.position.y, transform.position.z,
			target.x, target.y, target.z,
			0, 1, 0);
	}
};
