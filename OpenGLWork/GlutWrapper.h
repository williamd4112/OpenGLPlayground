#pragma once

#include <iostream>

typedef void (*DisplayFunc)();
typedef void (*TimerFunc)(int);
typedef void (*ReshapeFunc)(int, int);
typedef void (*KeyboardFunc)(unsigned char, int, int);
typedef void (*SpecialFunc)(int, int, int);
typedef void (*MouseFunc)(int, int, int, int);
typedef void (*MouseWheelFunc)(int, int, int, int);
typedef void (*MouseMotionFunc)(int, int);

class GlutWrapper
{
public:
	enum ExceptionType
	{
		INSTANCE_EXIST,
		NOT_YET_INIT,
		SETUP_FAIL
	};

	struct GlutWrapperException
	{
		ExceptionType type;
		std::string message;

		GlutWrapperException(ExceptionType _type, const char *_msg = "(No Message)")
			: type(_type), message(_msg)
		{
		
		}
	};

	~GlutWrapper();

	static void Setup(int *argc, char **argv, int _x, int _y, int _w, int _h, float _frameRate);
	static GlutWrapper &GetInstance();
	static void SetDisplayFunction(DisplayFunc);
	static void SetTimerFunction(TimerFunc);
	static void SetReshapeFunction(ReshapeFunc);
	static void SetKeyboardFunction(KeyboardFunc);
	static void SetSpecialFunction(SpecialFunc);
	static void SetMouseFunction(MouseFunc);
	static void SetMouseWheelFunction(MouseWheelFunc);
	static void SetMouseMotionFunction(MouseMotionFunc);
	static void Start();
	static void Close();
	static void Display();
	static void Timer(int t);
	static void Reshape(int w, int h);
	static void Keyboard(unsigned char key, int x, int y);
	static void Special(int key, int x, int y);
	static void Mouse(int button, int state, int x, int y);
	static void MouseWheel(int wheel, int position, int x, int y);
	static void MouseMotion(int x, int y);
private:
	static GlutWrapper *instance;
	static std::string name;
	static int x, y, width, height;
	static float frameRate;
	static DisplayFunc displayFunc;
	static TimerFunc timerFunc;
	static ReshapeFunc reshapeFunc;
	static KeyboardFunc keyboardFunc;
	static SpecialFunc specialFunc;
	static MouseFunc mouseFunc;
	static MouseWheelFunc mouseWheelFunc;
	static MouseMotionFunc mouseMotionFunc;

	GlutWrapper();
};

