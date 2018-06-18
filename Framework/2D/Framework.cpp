#include <sstream>
#include <algorithm> // For using std::max
#include <GLFW/glfw3.h>

#include <Framework/Framework.h>

#include <Base.h>
#include <physicsWorld.h>
#include <Renderer/Renderer.h>
#include <DemoUtils.h>

#include <Scene.h>

#include <physicsViewer.h>

struct RenderConfig
{
	unsigned int windowWidth;
	unsigned int windowHeight;
	unsigned int fps; // TODO: actually make use of this variable
};

struct MousePickingContext
{
	MousePickingContext( std::shared_ptr<physicsWorld> world )
	{
		arm.setZero();
		bodyId = invalidId;
		this->world = world;
	}

	Vector4 arm; // Body-local grabbed position
	int bodyId; // Id of body grabbed
	DemoUtils::ControlInfo controlInfo; // 
	std::shared_ptr<physicsWorld> world; // Physics world simulating the body
};

struct KeyPressContext
{
	KeyPressContext()
	{
		escPressed = spacePressed = enterPressed = false;
	}

	// Flags used to determine first-time key presses
	bool escPressed;
	bool spacePressed;
	bool enterPressed;
};

enum State
{
	Running,
	Paused
};

void transformPointGLFWtoGL( GLFWwindow* window, double xPos, double yPos, Vector4& pointGL )
{
	float left, right, bottom, top;
	getDimensions( left, right, bottom, top );

	int windowWidth, windowHeight;
	glfwGetWindowSize( window, &windowWidth, &windowHeight );

	// Reflect monitor-space across x-axis
	Vector4 axisX( 1.f, 0.f );
	Transform axisXReflection; axisXReflection.setReflection( axisX );

	Vector4 pointGLFW( xPos, yPos );
	pointGL.setTransformedPos( axisXReflection, pointGLFW );

	// Shift origin back to zero
	Vector4 offsetWindowHeight( 0.f, windowHeight );
	Transform shiftWindowHeight( offsetWindowHeight, 0.f );
	pointGL.setTransformedPos( shiftWindowHeight, pointGL );

	// Scale to dimension-space
	Vector4 scale( ( right - left ) / windowWidth, ( top - bottom ) / windowHeight );
	pointGL *= scale;

	// Shift origin to dimension-space
	Vector4 bottomLeft( left, bottom );
	Transform tBottomLeft( bottomLeft, 0.f );
	pointGL.setTransformedPos( tBottomLeft, pointGL );
}

void handleCursorPosition( GLFWwindow* window, MousePickingContext* context )
{
	double xPos, yPos; glfwGetCursorPos( window, &xPos, &yPos );
	Vector4 posGL; transformPointGLFWtoGL( window, xPos, yPos, posGL );

	if ( context->bodyId != invalidId )
	{
		DemoUtils::grab( context->controlInfo, context->world, context->bodyId, posGL );
	}
}

void handleMouseButton( GLFWwindow* window, MousePickingContext* context )
{
	int leftMouseButtonAction = glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_LEFT );

	double xPos, yPos; glfwGetCursorPos( window, &xPos, &yPos );
	Vector4 posGL; transformPointGLFWtoGL( window, xPos, yPos, posGL );

	if ( leftMouseButtonAction == GLFW_PRESS && context->bodyId == invalidId )
	{
		HitResult hitResult;
		context->world->queryPoint( posGL, hitResult );

		if ( hitResult.numHits > 0 )
		{
			// Just consider the first hit body
			const HitResult::HitInfo& firstHit = *hitResult.hitInfos.begin();

			if ( context->world->getMotionType( firstHit.bodyId ) == physicsMotionType::DYNAMIC )
			{
				context->bodyId = firstHit.bodyId;
				context->arm = firstHit.hitPos;
				DemoUtils::grab( context->controlInfo, context->world, context->bodyId, posGL );
			}
		}
	}
	else if ( leftMouseButtonAction == GLFW_RELEASE && context->bodyId != invalidId )
	{
		DemoUtils::release( context->controlInfo, context->world, context->bodyId );
		context->bodyId = invalidId;
		context->arm.setZero();
	}
}

void drawAxis()
{
	float left, right, bottom, top;
	getDimensions( left, right, bottom, top );

	Vector4 xleft( left, 0.f );
	Vector4 xright( right, 0.f );
	Vector4 ybottom( 0.f, bottom );
	Vector4 ytop( 0.f, top );

	drawLine( xleft, xright ); // X-axis
	drawLine( ybottom, ytop ); // Y-axis

	const int interval = 50;

	float shift = 8.f;
	Vector4 base, ext;
	Vector4 offset( shift, shift );

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

void drawCursorMarker( GLFWwindow* window )
{
	double xPos, yPos; glfwGetCursorPos( window, &xPos, &yPos );
	Vector4 posGL; transformPointGLFWtoGL( window, xPos, yPos, posGL );
	drawCross( posGL, 45.f * g_degToRad, 50.f, PURPLE );
}

void stepFramework( GLFWwindow* window, MousePickingContext* mousePickingContext )
{
	glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );
	glColor3f( 0.0f, 0.0f, 0.0f );

	drawAxis();
	drawCursorMarker( window );

	// Step physics
    // TODO: don't depend on mousePickingContext for stepping world
	mousePickingContext->world->step();

	// Step visualizers
	physicsViewer::viewShapes( mousePickingContext->world.get() );
	//physicsViewer::viewBroadphase( mousePickingContext->world.get() );

	// Step framework controls
	handleCursorPosition( window, mousePickingContext );
	handleMouseButton( window, mousePickingContext );

	// Draw
	stepRenderer();
	glfwSwapBuffers( window );
}

int initWindow( GLFWwindow*& window, const RenderConfig& cinfo )
{
	if ( !glfwInit() )
	{
		return -1;
	}

	window = glfwCreateWindow( cinfo.windowWidth, cinfo.windowHeight, "Simulation", nullptr, nullptr );

	Assert( window, "glfwCreateWindow failed" );

	glfwMakeContextCurrent( window );

	glfwWindowHint( GLFW_SAMPLES, 4 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 2 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1 );

	return 0;
}

int initPhysicsMathSettings()
{
	// Raise floating point exceptions
	// unsigned int currentState = 0;
	// _controlfp_s( &currentState, 0, 0 );
	// _controlfp_s( &currentState, _EM_INEXACT | _EM_UNDERFLOW | _EM_OVERFLOW | _EM_ZERODIVIDE | _EM_INVALID, _MCW_EM );

	_mm_setcsr( _mm_getcsr() | 0x8040 );

	return 0;
}

void handleKeyPress( GLFWwindow* window, State* state, KeyPressContext* keyPressContext, MousePickingContext* mousePickingContext )
{
	if ( glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_PRESS && !keyPressContext->escPressed )
	{
		keyPressContext->escPressed = true;
		glfwSetWindowShouldClose( window, GL_TRUE );
		return;
	}
	else if ( glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_RELEASE )
	{
		keyPressContext->escPressed = false;
	}

	if ( glfwGetKey( window, GLFW_KEY_SPACE ) == GLFW_PRESS && !keyPressContext->spacePressed )
	{
		keyPressContext->spacePressed = true;
		if ( *state == Paused )
		{
			stepFramework( window, mousePickingContext);
		}
		return;
	}
	else if ( glfwGetKey( window, GLFW_KEY_SPACE ) == GLFW_RELEASE )
	{
		keyPressContext->spacePressed = false;
	}

	if ( glfwGetKey( window, GLFW_KEY_ENTER ) == GLFW_PRESS && !keyPressContext->enterPressed )
	{
		keyPressContext->enterPressed = true;
		if ( *state == Running )
		{
			*state = Paused;
			return;
		}

		if ( *state == Paused )
		{
			*state = Running;
			return;
		}
	}
	else if ( glfwGetKey( window, GLFW_KEY_ENTER ) == GLFW_RELEASE )
	{
		keyPressContext->enterPressed = false;
	}
}

void startSimulationAndRender( int argc, char* argv[] )
{
	// Set control bits
	initPhysicsMathSettings();

	// Setup physics world with bodies
	std::shared_ptr<physicsWorld> world = initPhysicsScene();

	RenderConfig cinfo;
	{
		cinfo.windowWidth = 1366; // Taken arbitrarily from my own machine
		cinfo.windowHeight = 768;
		cinfo.fps = 30;
	}

	Assert( cinfo.windowWidth > 0, "Width of window is negative" );
	Assert( cinfo.windowHeight > 0, "Height of window is negative" );

	GLFWwindow* window;
	initWindow( window, cinfo );
	initRenderer( cinfo.windowWidth, cinfo.windowHeight );

	glfwPollEvents();

	MousePickingContext mousePickingContext( world );
	KeyPressContext keyPressContext;
	State state = Paused;

	stepFramework( window, &mousePickingContext );

	while ( !glfwWindowShouldClose( window ) )
	{
		handleKeyPress( window, &state, &keyPressContext, &mousePickingContext );

		if ( state == Running )
		{
			glfwPollEvents();
			stepFramework( window, &mousePickingContext );
		}
		else if ( state == Paused )
		{
			glfwWaitEvents();
		}
	}

	closeRenderer();

	glfwDestroyWindow( window );

	glfwTerminate();

	exit( EXIT_SUCCESS );
}