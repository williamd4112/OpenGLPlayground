#include <iostream>

#include "GLMeshObject.h"
#include "GLPerspectiveCamera.h"
#include "GLLegacyLight.h"

#include "GlutWrapper.h"


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
static void Reshape(int w, int h);
static void Timer(int t);
static void Keyboard(unsigned char key, int x, int y);
static void Special(int key, int x, int y);
static void MouseWheel(int, int, int, int);

static GLPerspectiveCamera camera(60.0f, 0.3f, 1000.0f, (640 / 480));
static GLLegacyLight light(
	glm::vec4(0.01, 0.01, 0.01, 0.1),
	glm::vec4(0.6, 0.6, 0.6, 0.6),
	glm::vec4(1.0, 1.0, 1.0, 1.0),
	glm::vec4(5.0, 5.0, 5.0, 1.0));

static GLMeshObject mesh;

int main(int argc, char *argv[])
{
	try {
		GlutWrapper::Setup(&argc, argv, 0, 0, 640, 480, 1.0f / 30.0f);
		GlutWrapper::SetDisplayFunction(Display);
		GlutWrapper::SetReshapeFunction(Reshape);
		GlutWrapper::SetTimerFunction(Timer);
		GlutWrapper::SetKeyboardFunction(Keyboard);
		GlutWrapper::SetSpecialFunction(Special);
		GlutWrapper::SetMouseWheelFunction(MouseWheel);
		
		mesh.Load("resource/Robot Warrior/Robot Warrior.obj");
		camera.GetTransform().Translate(glm::vec3(0, 0, 10));
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
	glShadeModel(GL_SMOOTH);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

	camera.RenderFixedPipeline();
	light.Setup(GL_LIGHT0);

	mesh.RenderFixedPipeline();
	
	camera.FinishRenderFixedPipeline();

	glDisable(GL_DEPTH_TEST);
}

static void Reshape(int w, int h)
{
	camera.GetConfig().aspect = w / h;
}

static void Timer(int t)
{
	float time = glutGet(GLUT_ELAPSED_TIME) * 0.001f;
	light.position.x = 10 *std::cos(time);
	light.position.z = 10 *std::sin(time);
	glutPostRedisplay();
}

static void Keyboard(unsigned char key, int x, int y)
{
	static float scalar = 0.1;
	glm::vec3 forward = camera.GetTransform().Orientation();

	switch (key)
	{
	case 'A':case'a':
		mesh.GetTransform().Translate(glm::vec3(-scalar, 0, 0));
		break;
	case 'D':case'd':
		mesh.GetTransform().Translate(glm::vec3(scalar, 0, 0));
		break;
	case 'W':case'w':
		mesh.GetTransform().Translate(glm::vec3(0, scalar, 0));
		break;
	case 'S':case's':
		mesh.GetTransform().Translate(glm::vec3(0, -scalar, 0));
		break;
	case 'Q':case'q':
		mesh.GetTransform().Rotate(glm::vec3(0, -scalar, 0));
		break;
	case 'E':case'e':
		mesh.GetTransform().Rotate(glm::vec3(0, scalar, 0));
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

static void MouseWheel(int wheel, int position, int x, int y)
{
	static float scalar = 0.1f;
	Transform &transform = camera.GetTransform();
	if (position < 0)
		transform.Translate(transform.Orientation() * scalar);
	else if (position > 0)
		transform.Translate(transform.Orientation() * -scalar);
}