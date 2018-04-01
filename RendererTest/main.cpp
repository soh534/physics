#include <Renderer/Renderer/Renderer.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

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

	while ( !glfwWindowShouldClose( window ) )
	{
		glfwPollEvents();

		glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		renderer.drawBox( Vector4( 100.f, 100.f ), Vector4( 300.f, 300.f ), BLUE );
		renderer.drawText( "bbbbbbb", Vector4( 150.f, 150.f ), 1.f, BLUE );

		

		renderer.step();

		glfwSwapBuffers( window );
	}

	glfwDestroyWindow( window );

	glfwTerminate();

	exit( EXIT_SUCCESS );

	return 0;
}