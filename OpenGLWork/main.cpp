#include <iostream>
#include <fstream>

#include "GLMeshObject.h"
#include "GlutWrapper.h"
#include "GLPerspectiveCamera.h"
#include "GLLegacyLight.h"
#include "GLShaderPipeline.h"
#include "GLShaderLight.h"
#include "GLKeyFrameAnimation.h"
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#define DEFAULT_WIDTH 640
#define DEFAULT_HEIGHT 480

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
static inline void FunctionAnimation(float angle);

static void Display();
static void DisplayShader();
static void Reshape(int w, int h);
static void Timer(int t);
static void Keyboard(unsigned char key, int x, int y);
static void Special(int key, int x, int y);
static void MouseWheel(int, int, int, int);

static GLPerspectiveCamera camera(60.0f, 0.3f, 1000.0f, (DEFAULT_WIDTH / DEFAULT_HEIGHT));
static GLLegacyLight light(
	glm::vec4(0.01, 0.01, 0.01, 0.6),
	glm::vec4(0.6, 0.6, 0.6, 0.8),
	glm::vec4(1.0, 1.0, 1.0, 13.0),
	glm::vec4(5.0, 5.0, 5.0, 5.0));

static std::vector<GLMeshObject*> meshes;

static Transform origin(vec3(0, 0, 0), vec3(0, 0, 0), vec3(1, 1, 1));

static double timeScale = 0.01f;

GLMeshObject 
*head, *helmet, *sword,
*body,
*rightArm, *leftArm,
*rightHand, *leftHand,
*lowerBody,
*rightLeg, *leftLeg,
*rightFoot, *leftFoot;

static GLKeyFrameAnimation anim;

static void LoadModel()
{
	body = new GLMeshObject;
	body->Load("Body.obj");

	head = new GLMeshObject;
	head->Load("Head.obj");
	head->GetTransform().Translate(glm::vec3(
		0, body->Mesh().GetBound().Height() * 0.2, body->Mesh().GetBound().Length() * 0.2));
	body->AddChild(head);

	helmet = new GLMeshObject;
	helmet->Load("Helmet.obj");
	helmet->GetTransform().Translate(glm::vec3(0, head->Mesh().GetBound().Height() - .05, 0.05));
	head->AddChild(helmet);

	rightArm = new GLMeshObject;
	rightArm->Load("Right_Arm.obj");
	rightArm->GetTransform().Translate(glm::vec3(-body->Mesh().GetBound().Width() / 2 + 0.2,
		0.25, 0));
	body->AddChild(rightArm);
	rightHand = new GLMeshObject;
	rightHand->Load("Right_Hand.obj");
	rightHand->GetTransform().Translate(glm::vec3(
		-rightArm->Mesh().GetBound().Width() + 0.3,
		-0.08, -0.05));
	rightArm->AddChild(rightHand);

	leftArm = new GLMeshObject;
	leftArm->Load("left_Arm.obj");
	leftArm->GetTransform().Translate(glm::vec3(
		body->Mesh().GetBound().Width() / 2 - 0.2,
		0.25, 0));
	body->AddChild(leftArm);
	leftHand = new GLMeshObject;
	leftHand->Load("left_Hand.obj");
	leftHand->GetTransform().Translate(glm::vec3(
		leftArm->Mesh().GetBound().Width() - 0.3,
		-0.08, -0.05));
	leftArm->AddChild(leftHand);

	lowerBody = new GLMeshObject;
	lowerBody->Load("LowerBody.obj");
	lowerBody->GetTransform().Translate(
		vec3(0,
			-body->Mesh().GetBound().Height() / 2 - 0.25,
			0));
	body->AddChild(lowerBody);
	rightLeg = new GLMeshObject;
	rightLeg->Load("Right_UpperLeg.obj");
	rightLeg->GetTransform().Translate(vec3(
		-lowerBody->Mesh().GetBound().Width() / 3,
		-lowerBody->Mesh().GetBound().Height() * 0.1,
		0));
	lowerBody->AddChild(rightLeg);
	rightFoot = new GLMeshObject;
	rightFoot->Load("Right_Foot.obj");
	rightFoot->GetTransform().Translate(vec3(
		-rightLeg->Mesh().GetBound().Width() / 2,
		-rightLeg->Mesh().GetBound().Height() / 2 - 0.08,
		rightLeg->Mesh().GetBound().Length() / 2));
	rightLeg->AddChild(rightFoot);

	leftLeg = new GLMeshObject;
	leftLeg->Load("Left_UpperLeg.obj");
	leftLeg->GetTransform().Translate(vec3(
		lowerBody->Mesh().GetBound().Width() / 3,
		-lowerBody->Mesh().GetBound().Height() * 0.1,
		0));
	lowerBody->AddChild(leftLeg);
	leftFoot = new GLMeshObject;
	leftFoot->Load("Left_Foot.obj");
	leftFoot->GetTransform().Translate(vec3(
		leftLeg->Mesh().GetBound().Width() / 2,
		-leftLeg->Mesh().GetBound().Height() / 2 - 0.08,
		leftLeg->Mesh().GetBound().Length() / 2));
	leftLeg->AddChild(leftFoot);

	sword = new GLMeshObject;
	sword->Load("resource/Robot Warrior/Sword.obj");
	sword->GetTransform().Translate(vec3(-1.0, 0, -0.8));
	sword->GetTransform().Rotate(vec3(90, 0, 0));
	sword->GetTransform().Scale(vec3(1, 1, 1.2));
	rightHand->AddChild(sword);

	meshes.push_back(body);
}

static void rightArmAnim()
{
	anim.AddKeyFrame(rightArm,
		GLKeyFrameAnimation::KeyFrame(0,
			Transform(vec3(rightArm->GetTransform().Position()),
				vec3(0, 0, 75),
				vec3(1, 1, 1)), ROTATION));
	anim.AddKeyFrame(rightArm,
		GLKeyFrameAnimation::KeyFrame(3,
			Transform(vec3(rightArm->GetTransform().Position()),
				vec3(60, 0, 75),
				vec3(1, 1, 1)), ROTATION));
	anim.AddKeyFrame(rightArm,
		GLKeyFrameAnimation::KeyFrame(6,
			Transform(vec3(rightArm->GetTransform().Position()),
				vec3(-60, 0, 75),
				vec3(1, 1, 1)), ROTATION));
	anim.AddKeyFrame(rightArm,
		GLKeyFrameAnimation::KeyFrame(9,
			Transform(vec3(rightArm->GetTransform().Position()),
				vec3(0, 0, 75),
				vec3(1, 1, 1)), ROTATION));
}

static void rightHandAnim()
{
	anim.AddKeyFrame(rightHand,
		GLKeyFrameAnimation::KeyFrame(0,
			Transform(vec3(rightHand->GetTransform().Position()),
				vec3(0, 90, 0),
				vec3(1, 1, 1)), ROTATION));
}

static void leftArmAnim()
{
	anim.AddKeyFrame(leftArm,
		GLKeyFrameAnimation::KeyFrame(0,
			Transform(vec3(leftArm->GetTransform().Position()),
				vec3(0, 0, -75),
				vec3(1, 1, 1)), ROTATION));
	anim.AddKeyFrame(leftArm,
		GLKeyFrameAnimation::KeyFrame(3,
			Transform(vec3(leftArm->GetTransform().Position()),
				vec3(-60, 0, -75),
				vec3(1, 1, 1)), ROTATION));
	anim.AddKeyFrame(leftArm,
		GLKeyFrameAnimation::KeyFrame(6,
			Transform(vec3(leftArm->GetTransform().Position()),
				vec3(60, 0, -75),
				vec3(1, 1, 1)), ROTATION));
	anim.AddKeyFrame(leftArm,
		GLKeyFrameAnimation::KeyFrame(9,
			Transform(vec3(leftArm->GetTransform().Position()),
				vec3(0, 0, -75),
				vec3(1, 1, 1)), ROTATION));
}

static void leftHandAnim()
{
	anim.AddKeyFrame(leftHand,
		GLKeyFrameAnimation::KeyFrame(0,
			Transform(vec3(leftHand->GetTransform().Position()),
				vec3(0, -90, 0),
				vec3(1, 1, 1)), ROTATION));
}

static void rightLegAnim()
{
	anim.AddKeyFrame(rightLeg, 0);
	anim.AddKeyFrame(rightLeg,
		GLKeyFrameAnimation::KeyFrame(2,
			Transform(vec3(rightLeg->GetTransform().Position()),
				vec3(-90, 0, 0),
				vec3(1, 1, 1)), ROTATION));
	anim.AddKeyFrame(rightLeg,
		GLKeyFrameAnimation::KeyFrame(6,
			Transform(vec3(rightLeg->GetTransform().Position()),
				vec3(20, 0, 0),
				vec3(1, 1, 1)), ROTATION));
	anim.AddKeyFrame(rightLeg, 9);
}

static void rightFootAnim()
{
	anim.AddKeyFrame(rightFoot, 0);
	anim.AddKeyFrame(rightFoot,
		GLKeyFrameAnimation::KeyFrame(2,
			Transform(vec3(rightFoot->GetTransform().Position()),
				vec3(75, 0, 0),
				vec3(1, 1, 1)), ROTATION));
	anim.AddKeyFrame(rightFoot,
		GLKeyFrameAnimation::KeyFrame(6,
			Transform(vec3(rightFoot->GetTransform().Position()),
				vec3(75, 0, 0),
				vec3(1, 1, 1)), ROTATION));
	anim.AddKeyFrame(rightFoot, 9);
}

static void leftLegAnim()
{
	anim.AddKeyFrame(leftLeg, 0);
	anim.AddKeyFrame(leftLeg,
		GLKeyFrameAnimation::KeyFrame(2,
			Transform(vec3(leftLeg->GetTransform().Position()),
				vec3(20, 0, 0),
				vec3(1, 1, 1)), ROTATION));
	anim.AddKeyFrame(leftLeg,
		GLKeyFrameAnimation::KeyFrame(6,
			Transform(vec3(leftLeg->GetTransform().Position()),
				vec3(-90, 0, 0),
				vec3(1, 1, 1)), ROTATION));
	anim.AddKeyFrame(leftLeg, 9);
}

static void leftFootAnim()
{
	anim.AddKeyFrame(leftFoot, 0);
	anim.AddKeyFrame(leftFoot,
		GLKeyFrameAnimation::KeyFrame(2,
			Transform(vec3(leftFoot->GetTransform().Position()),
				vec3(75, 0, 0),
				vec3(1, 1, 1)), ROTATION));
	anim.AddKeyFrame(leftFoot,
		GLKeyFrameAnimation::KeyFrame(6,
			Transform(vec3(leftFoot->GetTransform().Position()),
				vec3(75, 0, 0),
				vec3(1, 1, 1)), ROTATION));

	anim.AddKeyFrame(leftFoot, 9);
}

static void bodyAnim()
{
	anim.AddKeyFrame(body,
		GLKeyFrameAnimation::KeyFrame(0,
			Transform(vec3(body->GetTransform().Position()),
				vec3(15, 0, 0),
				vec3(1, 1, 1)), ROTATION));
	anim.AddKeyFrame(body,
		GLKeyFrameAnimation::KeyFrame(3,
			Transform(vec3(body->GetTransform().Position()),
				vec3(15, 0, 0),
				vec3(1, 1, 1)), ROTATION));
	anim.AddKeyFrame(body,
		GLKeyFrameAnimation::KeyFrame(6,
			Transform(vec3(body->GetTransform().Position()),
				vec3(20, 0, 0),
				vec3(1, 1, 1)), ROTATION));
	anim.AddKeyFrame(body,
		GLKeyFrameAnimation::KeyFrame(9,
			Transform(vec3(body->GetTransform().Position()),
				vec3(15, 0, 0),
				vec3(1, 1, 1)), ROTATION));
}

static void lowerBodyAnim()
{
	anim.AddKeyFrame(lowerBody,
		GLKeyFrameAnimation::KeyFrame(0,
			Transform(vec3(lowerBody->GetTransform().Position()),
				vec3(0, 10, 0),
				vec3(1, 1, 1)), ROTATION));
	anim.AddKeyFrame(lowerBody,
		GLKeyFrameAnimation::KeyFrame(6,
			Transform(vec3(lowerBody->GetTransform().Position()),
				vec3(0, -10, 0),
				vec3(1, 1, 1)), ROTATION));
	anim.AddKeyFrame(lowerBody,
		GLKeyFrameAnimation::KeyFrame(9,
			Transform(vec3(lowerBody->GetTransform().Position()),
				vec3(0, 10, 0),
				vec3(1, 1, 1)), ROTATION));
}

static void headAnim()
{
	anim.AddKeyFrame(head, 0);
	anim.AddKeyFrame(head,
		GLKeyFrameAnimation::KeyFrame(3,
			Transform(vec3(head->GetTransform().Position()),
				vec3(0, 15, 0),
				vec3(1, 1, 1)), ROTATION));
	anim.AddKeyFrame(head,
		GLKeyFrameAnimation::KeyFrame(6,
			Transform(vec3(head->GetTransform().Position()),
				vec3(0, -15, 0),
				vec3(1, 1, 1)), ROTATION));
	anim.AddKeyFrame(head, 9);
}

int main(int argc, char *argv[])
{
	try {
		GlutWrapper::Setup(&argc, argv, 0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT, 1.0f / 30.0f);
		GlutWrapper::SetDisplayFunction(Display);
		GlutWrapper::SetReshapeFunction(Reshape);
		GlutWrapper::SetTimerFunction(Timer);
		GlutWrapper::SetKeyboardFunction(Keyboard);
		GlutWrapper::SetSpecialFunction(Special);
		GlutWrapper::SetMouseWheelFunction(MouseWheel);
		
		LoadModel();
		rightArmAnim();
		rightHandAnim();
		leftArmAnim();
		leftHandAnim();
		rightLegAnim();
		rightFootAnim();
		leftLegAnim();
		leftFootAnim();
		lowerBodyAnim();
		bodyAnim();
		headAnim();

		camera.GetTransform().Translate(glm::vec3(0, 0, 6));

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
	glCullFace(GL_FRONT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

	camera.RenderFixedPipeline();
	light.Setup(GL_LIGHT0);
	origin.PushTransformMatrix();
	for (int i = 0; i < meshes.size(); i++)
		meshes[i]->RenderFixedPipeline();
	camera.FinishRenderFixedPipeline();
	origin.PopTransformMatrix();

	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glDisable(GL_CULL_FACE);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
}

static void DisplayShader()
{
	glShadeModel(GL_SMOOTH);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_FRONT);
	//glEnableVertexAttribArray(0);
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	//glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
	//glDisableVertexAttribArray(0);


	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
}

static void Reshape(int w, int h)
{
	camera.GetConfig().aspect = w / h;
}

static void Timer(int t)
{
	float time = glutGet(GLUT_ELAPSED_TIME) * timeScale;
	float angle = time * 8.0f;
	double lightTime = time * 0.1f;
	double animTime = time;

	anim.PlayAnimation(animTime);

	light.position[0] = std::cos(lightTime) * 10;
	light.position[2] = std::sin(lightTime) * 10;
	light.position[3] = 0;

	glutPostRedisplay();
}

static void Keyboard(unsigned char key, int x, int y)
{
	static float scalar = 0.1;
	switch (key)
	{
	case 'A':case'a':
		origin.Translate(glm::vec3(-scalar, 0, 0));
		break;
	case 'D':case'd':
		origin.Translate(glm::vec3(scalar, 0, 0));
		break;
	case 'W':case'w':
		origin.Translate(glm::vec3(0, scalar, 0));
		break;
	case 'S':case's':
		origin.Translate(glm::vec3(0, -scalar, 0));
		break;
	case 'Q':case'q':
		origin.Rotate(glm::vec3(0, -scalar * 10, 0));
		break;
	case 'E':case'e':
		origin.Rotate(glm::vec3(0, scalar * 10, 0));
		break;
	case 'Z':case'z':
		origin.Rotate(glm::vec3(-scalar * 10, 0, 0));
		break;
	case 'C':case'c':
		origin.Rotate(glm::vec3(scalar * 10, 0, 0));
		break;
	default:
		break;
	}
}

static void Special(int key, int x, int y)
{
	static double scalar = 0.01;
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
	case GLUT_KEY_PAGE_DOWN:
		timeScale -= 0.001f;
		break;
	case GLUT_KEY_PAGE_UP:
		timeScale += 0.001f;
		break;
	default:
		break;
	}
}

static void MouseWheel(int wheel, int position, int x, int y)
{
	static float scalar = 0.08f;
	Transform &transform = camera.GetTransform();
	if (position < 0)
		transform.Translate(glm::vec3(0, 0, 1) * scalar);
	else if (position > 0)
		transform.Translate(glm::vec3(0, 0, 1) * -scalar);
}

static inline void FunctionAnimation(float angle)
{
	head->GetTransform().RotateTo(vec3(0, 20 * radians(cos(angle)), 0));
	body->GetTransform().RotateTo(vec3(5 * radians(sin(angle)), radians(360 * std::sin(angle * 0.01)), 0));
	rightArm->GetTransform().RotateTo(glm::vec3(std::cos(angle), 0, glm::radians(60.0)));
	rightHand->GetTransform().RotateTo(glm::vec3(glm::radians(30.0), radians(90.0 * abs(cos(angle))), 0));

	leftArm->GetTransform().RotateTo(glm::vec3(-std::cos(angle), 0, -glm::radians(60.0)));
	leftHand->GetTransform().RotateTo(glm::vec3(glm::radians(30.0), -radians(90.0 * abs(cos(angle))), 0));

	rightLeg->GetTransform().RotateTo(glm::vec3(-radians(std::cos(angle) * 50), 0, 0));
	rightFoot->GetTransform().RotateTo(glm::vec3(radians(abs(std::cos(angle)) * 90), 0, 0));

	leftLeg->GetTransform().RotateTo(glm::vec3(radians(std::cos(angle) * 50), 0, 0));
	leftFoot->GetTransform().RotateTo(glm::vec3(radians(abs(std::cos(angle)) * 90), 0, 0));
}