#include <Renderer/Renderer/Renderer.h>
#include <Renderer/Renderer/Camera.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>

namespace
{
	static float cameraSpeed;
	double xLast, yLast;
	bool firstClick;

	void scrollCallback( GLFWwindow* window, double x, double y )
	{
        cameraSpeed *= (float)std::pow( 1.2, y );
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

		if ( firstClick )
		{
			xLast = xPos;
			yLast = yPos;
			firstClick = false;
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
		firstClick = true;
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

	cameraSpeed = 0.01f;

	glfwSetScrollCallback( window, &scrollCallback );

    {
        Cuboid cuboid( Vertex3( -0.25f, -0.25f, -0.25f ), Vertex3( 0.25f, 0.25f, 0.25f ) );
        glm::mat4 model = glm::mat4( 1.f );
        model = glm::translate( model, glm::vec3( 0.25f, 0.25f, 0.25f ) );
        renderer.addDisplayCuboid( cuboid, model, Color( 1.f, 0.5f, 0.3f ) );
    }

    {
        Cuboid cuboid( Vertex3( -0.5f, -0.5f, -0.5f ), Vertex3( 0.5f, 0.5f, 0.5f ) );
        glm::mat4 model = glm::mat4( 1.f );
        model = glm::translate( model, glm::vec3( -0.5f, -0.5f, -0.5f ) );
        renderer.addDisplayCuboid( cuboid, model, Color( 1.f, 0.5f, 0.3f ) );
    }

	while ( !glfwWindowShouldClose( window ) )
	{
		glfwPollEvents();

		renderer.prestep();

		glClearColor( 0.f, 0.f, 0.f, 1.f ); // Black, full opacity
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		processKey( window, renderer.getCamera() );
		processMouse( window, renderer.getCamera() );

		renderer.render();

		glfwSwapBuffers( window );
	}

	renderer.terminate();

	glfwDestroyWindow( window );

	glfwTerminate();

	exit( EXIT_SUCCESS );

	return 0;
}
