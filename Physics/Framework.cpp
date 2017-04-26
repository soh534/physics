#include <Framework.hpp>

#include <Base.hpp>

#include <physicsWorld.hpp>
#include <physicsBody.hpp>

#include <Renderer.hpp>
#include <DemoUtils.hpp>

#include <GLFW/glfw3.h>

#include <sstream>
#include <algorithm> // For using std::max

int g_widthWindow;
int g_heightWindow;
unsigned int g_framesPerSecond;
Vector3 g_topLeft;

// Body holding
bool g_bodyHeld = false;
Vector3 g_arm; // Local
Vector3 g_cursor; // World
Vector3 g_cursorVel;
int g_bodyId = -1;

physicsWorld* g_world;
DemoUtils::ControlInfo g_controlInfo;

enum State
{
	Running,
	Paused
};

static State g_state;

static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

void transformGLFWToGLCoordinate(const Vector3& pointGlut, Vector3& pointGL)
{
	// Make transformation matrix inside constructor
	Vector3 xShiftHalf(-1.f * g_widthWindow / 2.f, 0.f);
	Matrix3 t0; t0.setTranslation(xShiftHalf);

	Vector3 yAxis(1.f, 0.f);
	Matrix3 t1; t1.setReflection(yAxis);

	Vector3 yShiftHalf(0.f, g_heightWindow / 2.f);
	Matrix3 t2; t2.setTranslation(yShiftHalf);

	pointGL.setTransformedPos(t0, pointGlut);
	pointGL.setTransformedPos(t1, pointGL);
	pointGL.setTransformedPos(t2, pointGL);
}

static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (g_bodyId >= 0)
	{
		Vector3 currPos;
		transformGLFWToGLCoordinate(Vector3((Real)xpos, (Real)ypos), currPos);
		g_cursorVel = (currPos - g_cursor) / g_world->getDeltaTime();
		g_cursor = currPos;
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	double x, y;
	glfwGetCursorPos(window, &x, &y);

	drawCross( Vector3( x, y ), 0.f, 10.f, RED ); // debug

	transformGLFWToGLCoordinate(Vector3((Real)x, (Real)y), g_cursor);

	drawCross( g_cursor, 0, 10.f, BLUE ); // debug

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !g_bodyHeld)
	{
		const std::vector<physicsBody*>& bodies = g_world->getBodies();
		
		for (int i = 0; i < (int)bodies.size(); ++i)
		{
			if (!bodies[i]) continue;

			physicsBody const * const body = bodies[i];
			
			if (body->isStatic()) continue;
			
			if (body->containsPoint(g_cursor))
			{
				g_bodyId = body->getBodyId();
				g_arm.setSub(g_cursor, body->getPosition());
				g_bodyHeld = true;
				break;
			}
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE && g_bodyHeld)
	{
		DemoUtils::releaseControl(g_controlInfo, g_world, g_bodyId);
		g_bodyId = -1;
		g_arm.setZero();
		g_bodyHeld = false;
	}
}

void stepRender(GLFWwindow* window)
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.0f, 0.0f, 0.0f);

	renderAxis();

	g_world->step();
	g_world->render();

	// Print total momentum
	const std::vector<physicsBody*> bodies = g_world->getBodies();
	Vector3 totLinMomentum(0.f, 0.f, 0.f);

	for (int i = 0; i < (int)bodies.size(); i++)
	{
		if (!bodies[i]) continue;
		totLinMomentum += bodies[i]->getLinearVelocity() * bodies[i]->getMass();
	}

	std::stringstream ss;
	ss << totLinMomentum(0) << ", " << totLinMomentum(1) << std::endl;
	drawText(ss.str(), g_topLeft);

	// Add body held line
	if (g_bodyHeld)
	{
		if (g_bodyId > -1)
		{
			DemoUtils::controlBody(g_controlInfo, g_world, g_bodyId, g_cursor);
		}
	}

	step();

	glfwSwapBuffers(window);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
			return;
		}

		if (key == GLFW_KEY_SPACE)
		{
			if (g_state == Paused)
			{
				stepRender(window);
			}
			return;
		}

		if (key == GLFW_KEY_ENTER)
		{
			if (g_state == Running)
			{
				g_state = Paused;
				return;
			}

			if (g_state = Paused)
			{
				g_state = Running;
				return;
			}
		}
	}
}

int initializeWindow(GLFWwindow*& window, const WindowCinfo& cinfo)
{
	if (!glfwInit())
	{
		return -1;
	}

	window = glfwCreateWindow(cinfo.widthWindow, cinfo.heightWindow, "Simulation", nullptr, nullptr);

	Assert(window, "glfwCreateWindow failed");

	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	return 0;
}

void BeginGraphics(const WindowCinfo& cinfo)
{
	Assert(cinfo.widthWindow > 0, "Width of window is negative");
	Assert(cinfo.heightWindow > 0, "Height of window is negative");

	g_widthWindow = cinfo.widthWindow;
	g_heightWindow = cinfo.heightWindow;
	g_framesPerSecond = cinfo.framesPerSecond;
	g_topLeft.set(-g_widthWindow / 3.f, g_heightWindow / 3.f);
	g_world = cinfo.world;
	g_state = Running;
	g_controlInfo.dummyBodyId = -1;
	g_controlInfo.dummyJointId = -1;

	GLFWwindow* window;

	initializeWindow(window, cinfo);

	initializeRendering(cinfo.widthWindow, cinfo.heightWindow);

	while (!glfwWindowShouldClose(window))
	{
		if (g_state == Running)
		{
			glfwPollEvents();
			stepRender(window);
		}
		else if (g_state == Paused)
		{
			glfwWaitEvents();
		}
	}

	closeRendering();

	glfwDestroyWindow(window);

	glfwTerminate();

	exit(EXIT_SUCCESS);
}