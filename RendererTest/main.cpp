#include <Renderer/Renderer/Renderer.h>
#include <Renderer/Renderer/Camera.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <algorithm>

namespace
{
	static float cameraSpeed;
	double xLast, yLast;
	bool firstMouse;

	void scrollCallback( class GLFWwindow* window, double x, double y )
	{
		cameraSpeed *= std::pow( 1.2, y );
	}
}

void processKey( GLFWwindow* window, Camera* camera )
{
	if ( glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS )
		camera->dolly( cameraSpeed );
	if ( glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS )
		camera->truck( -cameraSpeed );
	if ( glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS )
		camera->dolly( -cameraSpeed );
	if ( glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS )
		camera->truck( cameraSpeed );
	if ( glfwGetKey( window, GLFW_KEY_Q ) == GLFW_PRESS )
		camera->pedestal( cameraSpeed );
	if ( glfwGetKey( window, GLFW_KEY_Z ) == GLFW_PRESS )
		camera->pedestal( -cameraSpeed );
}

void processMouse( GLFWwindow* window, Camera* camera )
{
	int leftMouseButtonAction = glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_LEFT );

	if ( leftMouseButtonAction == GLFW_PRESS )
	{
		double xPos, yPos; glfwGetCursorPos( window, &xPos, &yPos );

		if ( firstMouse )
		{
			xLast = xPos;
			yLast = yPos;
			firstMouse = false;
		}

		double xoffset = xPos - xLast;
		double yoffset = yLast - yPos;

		xLast = xPos;
		yLast = yPos;

		float sensitivity = 0.1f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		camera->panAndTilt( xoffset, yoffset );
	}

	if ( leftMouseButtonAction == GLFW_RELEASE )
	{
		firstMouse = true;
	}
}

int main( int argc, char* argv[] )
{
	GLFWwindow* window;

	if ( !glfwInit() )
	{
		return -1;
	}

	int width = 1024;
	int height = 768;

	window = glfwCreateWindow( width, height, "Renderer test", nullptr, nullptr );

	Assert( window, "glfwCreateWindow failed" );

	glfwMakeContextCurrent( window );

	Renderer renderer;
	renderer.init( width, height );

	cameraSpeed = 10.f;

	glfwSetScrollCallback( window, &scrollCallback );

	while ( !glfwWindowShouldClose( window ) )
	{
		glfwPollEvents();

		glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		renderer.drawBox( Vector4( 100.f, 100.f ), Vector4( 300.f, 300.f ), BLUE );
		renderer.drawText( "bbbbbbb", Vector4( 150.f, 150.f ), 1.f, BLUE );
		renderer.drawTriangle( Vector4( -100.f, -100.f ), Vector4( 100.f, 100.f ), Vector4( -100.f, 100.f ), RED );
		//renderer.drawTriangle( Vector4( -100.f, -100.f ), Vector4( 100.f, -100.f ), Vector4( 100.f, 100.f ), RED );
		
		processKey( window, renderer.getCamera() );
		processMouse( window, renderer.getCamera() );
		renderer.step();

		glfwSwapBuffers( window );
	}

	glfwDestroyWindow( window );

	glfwTerminate();

	exit( EXIT_SUCCESS );

	return 0;
}
