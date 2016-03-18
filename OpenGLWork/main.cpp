#include <iostream>

#include "GlutWrapper.h"
#include "GLPerspectiveCamera.h"
#include "Mesh.h"

static inline void ExceptionHandler(GlutWrapper::GlutWrapperException &e)
{
	switch (e.type)
	{
	case GlutWrapper::INSTANCE_EXIST:
		std::cerr << "Glut instance has been exist." << std::endl;
		break;
	case GlutWrapper::NOT_YET_INIT:
		std::cerr << "Glut instance doesn't exist." << std::endl;
		break;
	case GlutWrapper::SETUP_FAIL:
		std::cerr << "Setup failed. Error: " << e.message.c_str() << std::endl;
		break;
	default:
		std::cerr << "Unknown error." << std::endl;
		break;
	}
}

static void Display();
static void Timer(int t);
static void Keyboard(unsigned char key, int x, int y);
static void Special(int key, int x, int y);

static GLPerspectiveCamera camera(60.0f, 0.3f, 1000.0f, (640 / 480));

int main(int argc, char *argv[])
{
	try {
		GlutWrapper::Setup(&argc, argv, 0, 0, 640, 480, 1.0f / 30.0f);
		GlutWrapper::SetDisplayFunction(Display);
		GlutWrapper::SetTimerFunction(Timer);
		GlutWrapper::SetKeyboardFunction(Keyboard);
		GlutWrapper::SetSpecialFunction(Special);
		
		camera.GetTransform().Translate(glm::vec3(0, 0, 5));
		camera.GetTransform().Rotate(glm::vec3(0.2, 0, 0));

		GlutWrapper::Start();
		GlutWrapper::Close();
	}
	catch (GlutWrapper::GlutWrapperException e)
	{
		ExceptionHandler(e);
	}

	return 0;
}

static void Display()
{
	glColor3f(1, 1, 1);
	glEnable(GL_DEPTH_TEST);
	camera.RenderFixedPipeline();
	glutSolidSphere(0.5f, 100, 100);
	camera.FinishRenderFixedPipeline();
	glDisable(GL_DEPTH_TEST);
}

static void Timer(int t)
{
	glutPostRedisplay();
}

static void Keyboard(unsigned char key, int x, int y)
{
	static double scalar = 0.1;
	switch (key)
	{
	case 'A':case'a':
		camera.GetTransform().Translate(glm::vec3(-scalar, 0, 0));
		break;
	case 'D':case'd':
		camera.GetTransform().Translate(glm::vec3(scalar, 0, 0));
		break;
	case 'W':case'w':
		camera.GetTransform().Translate(glm::vec3(0, 0, -scalar));
		break;
	case 'S':case's':
		camera.GetTransform().Translate(glm::vec3(0, 0, scalar));
		break;
	default:
		break;
	}
}

static void Special(int key, int x, int y)
{
	static double scalar = 0.1;
	switch (key)
	{
	case GLUT_KEY_UP:
		camera.GetTransform().Rotate(glm::vec3(scalar, 0, 0));
		break;
	case GLUT_KEY_DOWN:
		camera.GetTransform().Rotate(glm::vec3(-scalar, 0, 0));
		break;
	case GLUT_KEY_LEFT:
		camera.GetTransform().Rotate(glm::vec3(0, -scalar, 0));
		break;
	case GLUT_KEY_RIGHT:
		camera.GetTransform().Rotate(glm::vec3(0, scalar, 0));
		break;
	default:
		break;
	}
}