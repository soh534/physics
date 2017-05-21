#include <sstream>
#include <algorithm> // For using std::max
#include <GLFW/glfw3.h>

#include <Base.h>
#include <physicsWorld.h>
#include <Renderer.h>
#include <DemoUtils.h>
#include <Framework.h>

int g_widthWindow;
int g_heightWindow;
unsigned int g_framesPerSecond;
Vector3 g_topLeft;

// Body holding
bool g_bodyHeld = false;
Vector3 g_arm; // Local
Vector3 g_cursorPos; // World
int g_bodyId = -1;

physicsWorld* g_world;
DemoUtils::ControlInfo g_controlInfo;

enum State
{
	Running,
	Paused
};

static State g_state;

static void error_callback( int error, const char* description )
{
	fputs( description, stderr );
}

void transformPointGLFWtoGL( const Vector3& pointGLFW, Vector3& pointGL )
{
	Vector3 yAxis( 1.f, 0.f );
	Matrix3 yAxisReflection; yAxisReflection.setReflection( yAxis );

	Vector3 windowHeight( 0.f, static_cast< Real >( g_heightWindow ) );
	Matrix3 shiftWindowHeight; shiftWindowHeight.setTranslation( windowHeight );

	pointGL.setTransformedPos( yAxisReflection, pointGLFW );
	pointGL.setTransformedPos( shiftWindowHeight, pointGL );
}

static void cursorPositionCallback( GLFWwindow* window, double xpos, double ypos )
{
	if ( g_bodyId >= 0 )
	{
		transformPointGLFWtoGL( Vector3( static_cast< Real >( xpos ), 
										 static_cast< Real >( ypos ) ),
								g_cursorPos );
	}
}

void mouseButtonCallback( GLFWwindow* window, int button, int action, int mods )
{
	double x, y;
	glfwGetCursorPos( window, &x, &y );

	transformPointGLFWtoGL( Vector3( (Real)x, (Real)y ), g_cursorPos );

	if ( button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !g_bodyHeld )
	{
		const std::vector<BodyId>& activeBodyIds = g_world->getActiveBodyIds();
		for ( auto i = 0; i < activeBodyIds.size(); i++ )
		{
			const physicsBody& body = g_world->getBody( activeBodyIds[i] );

			if ( body.isStatic() ) continue;

			if ( body.containsPoint( g_cursorPos ) )
			{
				g_bodyId = body.getBodyId();
				g_arm.setSub( g_cursorPos, body.getPosition() );
				g_bodyHeld = true;
				break;
			}
		}
	}
	else if ( button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE && g_bodyHeld )
	{
		DemoUtils::releaseControl( g_controlInfo, g_world, g_bodyId );
		g_bodyId = -1;
		g_arm.setZero();
		g_bodyHeld = false;
	}
}

void stepRender( GLFWwindow* window )
{
	glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );
	glColor3f( 0.0f, 0.0f, 0.0f );

	renderAxis();

	g_world->step();
	g_world->render();

	/// Print total momentum
	Vector3 totLinMomentum; totLinMomentum.setZero();
	const std::vector<BodyId>& activeBodyIds = g_world->getActiveBodyIds();
	for ( auto i = 0; i < activeBodyIds.size(); i++ )
	{
		const physicsBody& body = g_world->getBody( activeBodyIds[i] );
		totLinMomentum += body.getLinearVelocity() * body.getMass();
	}

	std::stringstream ss;
	ss << totLinMomentum( 0 ) << ", " << totLinMomentum( 1 ) << std::endl;
	drawText( ss.str(), g_topLeft );

	// Add body held line
	if ( g_bodyHeld )
	{
		if ( g_bodyId > -1 )
		{
			DemoUtils::controlBody( g_controlInfo, g_world, g_bodyId, g_cursorPos );
		}
	}

	step();

	glfwSwapBuffers( window );
}

static void key_callback( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	if ( action == GLFW_PRESS )
	{
		if ( key == GLFW_KEY_ESCAPE )
		{
			glfwSetWindowShouldClose( window, GL_TRUE );
			return;
		}

		if ( key == GLFW_KEY_SPACE )
		{
			if ( g_state == Paused )
			{
				stepRender( window );
			}
			return;
		}

		if ( key == GLFW_KEY_ENTER )
		{
			if ( g_state == Running )
			{
				g_state = Paused;
				return;
			}

			if ( g_state = Paused )
			{
				g_state = Running;
				return;
			}
		}
	}
}

int initializeWindow( GLFWwindow*& window, const WindowCinfo& cinfo )
{
	if ( !glfwInit() )
	{
		return -1;
	}

	window = glfwCreateWindow( cinfo.widthWindow, cinfo.heightWindow, "Simulation", nullptr, nullptr );

	Assert( window, "glfwCreateWindow failed" );

	glfwMakeContextCurrent( window );

	glfwSetKeyCallback( window, key_callback );
	glfwSetCursorPosCallback( window, cursorPositionCallback );
	glfwSetMouseButtonCallback( window, mouseButtonCallback );

	glfwWindowHint( GLFW_SAMPLES, 4 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 2 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1 );

	return 0;
}

void BeginGraphics( const WindowCinfo& cinfo )
{
	Assert( cinfo.widthWindow > 0, "Width of window is negative" );
	Assert( cinfo.heightWindow > 0, "Height of window is negative" );

	g_widthWindow = cinfo.widthWindow;
	g_heightWindow = cinfo.heightWindow;
	g_framesPerSecond = cinfo.framesPerSecond;
	g_topLeft.set( -g_widthWindow / 3.f, g_heightWindow / 3.f );
	g_world = cinfo.world;
	g_state = Running;
	g_controlInfo.dummyBodyId = -1;
	g_controlInfo.dummyJointId = -1;

	GLFWwindow* window;

	initializeWindow( window, cinfo );

	initializeRendering( cinfo.widthWindow, cinfo.heightWindow );

	while ( !glfwWindowShouldClose( window ) )
	{
		if ( g_state == Running )
		{
			glfwPollEvents();
			stepRender( window );
		}
		else if ( g_state == Paused )
		{
			glfwWaitEvents();
		}
	}

	closeRendering();

	glfwDestroyWindow( window );

	glfwTerminate();

	exit( EXIT_SUCCESS );
}