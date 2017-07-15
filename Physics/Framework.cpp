#include <sstream>
#include <algorithm> // For using std::max
#include <GLFW/glfw3.h>

#include <Base.h>
#include <physicsWorld.h>
#include <Renderer.h>
#include <DemoUtils.h>
#include <Framework.h>

/// Globally passed variables to GLFW callbacks
namespace
{
	/// Parameters related to holding bodies
	bool g_bodyHeld = false;
	Vector3 g_arm; // Local to body
	Vector3 g_cursorPos; // In world
	int g_bodyId = -1;
	DemoUtils::ControlInfo g_controlInfo;

	physicsWorld* g_world;
}

enum State
{
	Running,
	Paused
};

static State g_state;

void transformPointGLFWtoGL( GLFWwindow* window, const Vector3& pointGLFW, Vector3& pointGL )
{
	float left, right, bottom, top;
	getDimensions( left, right, bottom, top );

	int windowWidth, windowHeight;
	glfwGetWindowSize( window, &windowWidth, &windowHeight );

	/// Reflect monitor-space across x-axis
	Vector3 axisX( 1.f, 0.f );
	Transform axisXReflection; axisXReflection.setReflection( axisX );
	pointGL.setTransformedPos( axisXReflection, pointGLFW );

	/// Shift origin back to zero
	Vector3 offsetWindowHeight( 0.f, windowHeight );
	Transform shiftWindowHeight( offsetWindowHeight, 0.f );
	pointGL.setTransformedPos( shiftWindowHeight, pointGL );

	/// Scale to dimension-space
	Vector3 scale( ( right - left ) / windowWidth, ( top - bottom ) / windowHeight );
	pointGL *= scale;

	/// Shift origin to dimension-space
	Vector3 bottomLeft( left, bottom );
	Transform tBottomLeft( bottomLeft, 0.f );
	pointGL.setTransformedPos( tBottomLeft, pointGL );
}

static void cursorPositionCallback( GLFWwindow* window, double xpos, double ypos )
{
	Vector3 posGlfw( static_cast< Real >( xpos ), static_cast< Real >( ypos ) );
	transformPointGLFWtoGL( window, posGlfw, g_cursorPos );

	if ( g_bodyId >= 0 )
	{
		DemoUtils::controlBody( g_controlInfo, g_world, g_bodyId, g_cursorPos );
	}
}

void mouseButtonCallback( GLFWwindow* window, int button, int action, int mods )
{
	double x, y;
	glfwGetCursorPos( window, &x, &y );

	Vector3 posGlfw( static_cast< Real >( x ), static_cast< Real >( y ) );
	transformPointGLFWtoGL( window, posGlfw, g_cursorPos );

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
				DemoUtils::controlBody( g_controlInfo, g_world, g_bodyId, g_cursorPos );
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

void drawAxis()
{
	float left, right, bottom, top;
	getDimensions( left, right, bottom, top );

	Vector3 xleft( left, 0.f );
	Vector3 xright( right, 0.f );
	Vector3 ybottom( 0.f, bottom );
	Vector3 ytop( 0.f, top );

	drawLine( xleft, xright ); // X-axis
	drawLine( ybottom, ytop ); // Y-axis

	const int interval = 50;

	float shift = 8.f;
	Vector3 base, ext;
	Vector3 offset( shift, shift );

	for ( int i = 0; i < right; i += interval )
	{
		base.set( i, 0.f );
		ext.set( i, shift );
		drawLine( base, ext );
		drawText( std::to_string( i ), base + offset );
	}

	for ( int i = -interval; i > left; i -= interval )
	{
		base.set( i, 0.f );
		ext.set( i, shift );
		drawLine( base, ext );
		drawText( std::to_string( i ), base + offset );
	}

	for ( int i = interval; i < top; i += interval )
	{
		base.set( 0.f, i );
		ext.set( shift, i );
		drawLine( base, ext );
		drawText( std::to_string( i ), base + offset );
	}

	for ( int i = -interval; i > bottom; i -= interval )
	{
		base.set( 0.f, i );
		ext.set( shift, i );
		drawLine( base, ext );
		drawText( std::to_string( i ), base + offset );
	}
}

void drawCursorMarker()
{
	drawCross( g_cursorPos, 45.f * g_degToRad, 50.f, PURPLE );
}

void stepRender( GLFWwindow* window )
{
	glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );
	glColor3f( 0.0f, 0.0f, 0.0f );

	drawAxis();
	drawCursorMarker();

	/// Step physics
	g_world->render();
	g_world->step();

	/// Draw
	stepRenderer();
	glfwSwapBuffers( window );
}

static void keyCallback( GLFWwindow* window, int key, int scancode, int action, int mods )
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

	glfwSetKeyCallback( window, keyCallback );
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

	g_world = cinfo.world;
	g_state = Paused;
	g_controlInfo.dummyBodyId = -1;
	g_controlInfo.dummyJointId = -1;

	GLFWwindow* window;

	initializeWindow( window, cinfo );

	initializeRendering( cinfo.widthWindow, cinfo.heightWindow );

	glfwPollEvents();
	stepRender( window );

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