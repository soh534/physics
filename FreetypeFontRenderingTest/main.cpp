#include <Renderer.hpp>
#include <GLFW/glfw3.h>

int main( int argc, char* argv[] )
{
	GLFWwindow* window;

	if ( !glfwInit() )
	{
		return -1;
	}

	int width = 1024;
	int height = 768;

	window = glfwCreateWindow( width, height, "FreetypeTest", NULL, NULL );

	Assert( window, "glfwCreateWindow failed" );

	glfwMakeContextCurrent(window);

	initializeRendering( width, height );

	while ( !glfwWindowShouldClose( window ) )
	{
		glfwPollEvents();

		glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT );
		glColor3f( 0.0f, 0.0f, 0.0f );

		//drawBox( Vector3( 500.f, 500.f ), Vector3( 100.f, 100.f ), RED );
		drawText( "wow", Vector3( 250.f, 250.f ) );

		clearDisplays();

		glfwSwapBuffers( window );
	}

	closeRendering();

	glfwDestroyWindow( window );

	glfwTerminate();

	exit( EXIT_SUCCESS );

	return 0;
}