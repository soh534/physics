#include <Renderer/Renderer/Renderer.h>
#include <Renderer/Renderer/Camera.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>

//#include "SampleSceneTest.h"
#include "TextRenderingTest.h"

namespace
{
	static float cameraSpeed;
	double xLast, yLast;
	bool firstClickFlag;

	void scrollCallback( GLFWwindow* window, double x, double y )
	{
        cameraSpeed *= (float)std::pow( 1.2, y );
	}
}

void processKey( GLFWwindow* window, Camera* camera )
{
    // TODO: strange to be passing speed for amount
    if ( glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS )    camera->dolly( cameraSpeed );
    if ( glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS )	 camera->truck( -cameraSpeed );
	if ( glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS )    camera->dolly( -cameraSpeed );
	if ( glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS )	 camera->truck( cameraSpeed );
	if ( glfwGetKey( window, GLFW_KEY_Q ) == GLFW_PRESS )	 camera->pedestal( cameraSpeed );
	if ( glfwGetKey( window, GLFW_KEY_Z ) == GLFW_PRESS )	 camera->pedestal( -cameraSpeed );
}

void processMouse( GLFWwindow* window, Camera* camera )
{
	int leftMouseButtonAction = glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_LEFT );

	if ( leftMouseButtonAction == GLFW_PRESS )
	{
		double xPos, yPos; glfwGetCursorPos( window, &xPos, &yPos );

		if ( firstClickFlag )
		{
			xLast = xPos;
			yLast = yPos;
			firstClickFlag = false;
		}

		double xoffset = xPos - xLast;
		double yoffset = yLast - yPos;

		xLast = xPos;
		yLast = yPos;

		float sensitivity = 0.1f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

        camera->panAndTilt( (float)xoffset, (float)yoffset );
	}

	if ( leftMouseButtonAction == GLFW_RELEASE )
	{
		firstClickFlag = true;
	}
}

int main( int argc, char* argv[] )
{
	Assert( glfwInit() == GLFW_TRUE, "failed to initialize GLFW" );

	int width = 1024;
	int height = 768;

	GLFWwindow* window = glfwCreateWindow( width, height, "Renderer test", nullptr, nullptr );
	Assert( window, "glfwCreateWindow failed" );
	glfwMakeContextCurrent( window );

	Assert( glewInit() == GLEW_OK, "failed to initialize GLEW" );

    RendererCinfo cinfo;
	Renderer renderer;
	renderer.init( window, cinfo );

	cameraSpeed = 0.05f;

	glfwSetScrollCallback( window, &scrollCallback );

    Test::addObjects( renderer );

	while ( !glfwWindowShouldClose( window ) )
	{
		renderer.prestep();

		processKey( window, renderer.getCamera() );
		processMouse( window, renderer.getCamera() );

        Test::updateObjects( renderer );

		renderer.render();

		glfwSwapBuffers( window );
        glfwPollEvents();
	}

	renderer.terminate();

	glfwDestroyWindow( window );

	glfwTerminate();

	exit( EXIT_SUCCESS );

	return 0;
}
