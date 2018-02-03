#include <Renderer.h>
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

	window = glfwCreateWindow( width, height, "FreetypeTest", nullptr, nullptr );

	Assert( window, "glfwCreateWindow failed" );

	glfwMakeContextCurrent(window);

	initializeRendering( width, height );

	while ( !glfwWindowShouldClose( window ) )
	{
		glfwPollEvents();

		glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT );
		glColor3f( 0.0f, 0.0f, 0.0f );

		drawBox( Vector4( 500.f, 500.f ), Vector4( 100.f, 100.f ), RED );
		drawBox( Vector4( 250.f, 250.f ), Vector4( 50.f, 50.f ), BLUE );
		drawText( "bbbbbbb", Vector4( 25.f, 25.f ), BLUE );
		drawText( "wow", Vector4( 250.f, 250.f ) );
		drawText( "aaaaaaa", Vector4( 300.f, 300.f ) );
		drawText( "ccccccc", Vector4( 0.f, 500.f ), RED );
		drawText( "eeeeeee", Vector4( 0.f, 400.f ), LIME );
		drawText( "123456", Vector4( 0.f, 200.f ) );

		stepRenderer();

		glfwSwapBuffers( window );
	}

	closeRendering();

	glfwDestroyWindow( window );

	glfwTerminate();

	exit( EXIT_SUCCESS );

	return 0;
}