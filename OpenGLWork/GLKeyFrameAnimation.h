#pragma once

#include <vector>
#include <map>
#include <algorithm>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm\common.hpp>
#include "Transform.h"
#include "GLObject.h"

#define EMPTY 0x0
#define TRANSLATION 0x1
#define ROTATION 0x2
#define SCALE 0x4

typedef std::pair<GLObject *, Transform> TransformPair;
typedef unsigned int Tick;

class GLKeyFrameAnimation
{
public:
	struct KeyFrame
	{
		friend GLKeyFrameAnimation;
	public:
		KeyFrame(const Tick _tick, const Transform _transform = Transform(), 
			const unsigned int _type = EMPTY) :
			tick(_tick), type(_type), transformKey(_transform)
		{

		}

		friend bool operator <(const KeyFrame &a, const KeyFrame &b)
		{
			return a.tick < b.tick;
		}

		const Transform &GetKey() const { return transformKey; }
		const unsigned int GetType() const { return type; }

		static Transform Interpolate(const KeyFrame &a, const KeyFrame &b, double tick)
		{
			if (a.tick == b.tick) 
				return a.transformKey;

			const double val = (double)(tick - a.tick) / (double)(b.tick - a.tick);
			if (val < 0 || val >= 1)
				return b.transformKey;
			
			glm::vec3 newTranslation(glm::mix(glm::vec3(a.GetKey().Position())
				, b.GetKey().Position(), val));
			glm::vec3 newRotation(glm::mix(glm::vec3(a.GetKey().Rotation())
				, b.GetKey().Rotation(), val));
			glm::vec3 newScale(glm::mix(glm::vec3(a.GetKey().Scale())
				, b.GetKey().Scale(), val));
			return Transform(newTranslation, newRotation, newScale);
		}
	private:
		unsigned int type;
		Tick tick;
		Transform transformKey;
	};

	typedef std::map<GLObject *, std::vector<KeyFrame>> FramesMap;
	typedef std::vector<KeyFrame> FrameArray;

	GLKeyFrameAnimation();
	~GLKeyFrameAnimation();

	void PlayAnimation(double time);
	void AddKeyFrame(GLObject *obj, KeyFrame frame);
	void AddKeyFrame(GLObject *obj, unsigned int tick);
	int FindKeyFrame(const FrameArray &keyFrames, unsigned int tick);
private:
	unsigned int maxTick;
	FramesMap keyFramesMap;

};

