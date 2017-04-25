#define _USE_MATH_DEFINES
#include <math.h> // For M_PI
#include <cassert>

#include <physicsWorld.hpp>
#include <physicsBody.hpp>
#include <physicsShape.hpp>

#include <DemoUtils.hpp>

#include <Framework.hpp>

using namespace std;

physicsWorld* world;

int Simulation( int argc, char* argv[] )
{
	/// Raise floating point exceptions
	unsigned int currentState;
	_controlfp_s( &currentState, 0, _EM_UNDERFLOW | _EM_OVERFLOW | _EM_ZERODIVIDE | _EM_INVALID );

	physicsWorldCinfo cinfo;
	//cinfo.m_gravity.setZero();
	cinfo.m_gravity.set( 0.f, -98.1f );
	cinfo.m_numIter = 1;
	world = new physicsWorld( cinfo );

#if 0
	std::vector<physicsBody*> bodies;
	DemoUtils::createPackedCircles( bodies, 200 );

	for ( int i = 0; i < (int)bodies.size(); i++ )
	{
		world->addBody( bodies[ i ] );
	}
#endif

#if 0 /// TODO: Holding doesn't work properly on smaller box
	{
		physicsBodyCinfo cinfo;
		cinfo.m_shape = new physicsBoxShape( Vector3f( 60.0f, 60.0f ) );
		cinfo.m_pos.set( 180.0f, 180.0f );
		cinfo.m_rot = 30.0f * g_degToRad;
		cinfo.m_mass = 17.0f;
		cinfo.m_inertia = cinfo.m_mass;
		cinfo.m_linearVelocity.set( -10.f, -10.f );
		physicsBody* body = new physicsBody( cinfo );
		world->addBody( body );
	}

	{
		physicsBodyCinfo cinfo;
		cinfo.m_shape = new physicsBoxShape( Vector3f( 20.0f, 30.0f ) );
		cinfo.m_pos.set( -50.0f, -50.0f );
		cinfo.m_rot = 45.0f * g_degToRad;
		cinfo.m_mass = 7.0f;
		cinfo.m_inertia = cinfo.m_mass;
		cinfo.m_linearVelocity.set( 20.f, 20.f );
		physicsBody* body = new physicsBody( cinfo );
		world->addBody( body );
	}
#endif

#if 0
	{
		physicsBodyCinfo cinfo;
		cinfo.m_shape = new physicsCircleShape( 40.0f );
		cinfo.m_pos.set( 230.0f, 150.0f );
		cinfo.m_pos.set( -50.f, -50.f );
		cinfo.m_linearVelocity.set( 50.f, 50.f );
		/// cinfo.m_angularSpeed = 50.f * g_degToRad;
		physicsBody* body = new physicsBody( cinfo );
		world->addBody( body );
	}

	{
		physicsBodyCinfo cinfo;
		cinfo.m_shape = new physicsCircleShape( 60.0f );
		cinfo.m_pos.set( -230.0f, -150.0f );
		cinfo.m_pos.set( 50.f, 50.f );
		cinfo.m_linearVelocity.set( -10.f, -10.f );
		/// cinfo.m_angularSpeed = 100.f * g_degToRad;
		physicsBody* body = new physicsBody( cinfo );
		world->addBody( body );
	}
#endif

#if 1
#if 0
	int bIdA, bIdB;
	{
		physicsBodyCinfo cinfo;
		vector<Vector3> vertices;
		vertices.push_back( Vector3( -18.0f, 36.0f ) );
		vertices.push_back( Vector3( -30.0f, 6.0f ) );
		vertices.push_back( Vector3( 42.0, -6.0 ) );
		vertices.push_back( Vector3( -30.0f, -30.0f ) );
		vertices.push_back( Vector3( 12.0f, -60.0f ) );
		vertices.push_back( Vector3( 30.0f, 18.0f ) );
		cinfo.m_shape = new physicsConvexShape( vertices, .1f );
		cinfo.m_pos.set( -50.0f, -50.0f );
		/// cinfo.m_rot = 45.0f * g_degToRad;
		cinfo.m_linearVelocity.set( 50.0f, 50.0f );
		//cinfo.m_angularSpeed = 45.0f * g_degToRad;
		physicsBody* body = new physicsBody( cinfo );
		bIdA = world->addBody( body );
	}

	{
		physicsBodyCinfo cinfo;
		vector<Vector3> vertices;
		vertices.push_back( Vector3( -18.0f, 36.0f ) );
		vertices.push_back( Vector3( -30.0f, 6.0f ) );
		vertices.push_back( Vector3( 42.0, -6.0 ) );
		vertices.push_back( Vector3( -30.0f, -30.0f ) );
		vertices.push_back( Vector3( 12.0f, -60.0f ) );
		vertices.push_back( Vector3( 30.0f, 18.0f ) );
		cinfo.m_shape = new physicsConvexShape( vertices, .1f );
		cinfo.m_pos.set( 50.0f, 50.0f );
		/// cinfo.m_rot = 45.0f * g_degToRad;
		cinfo.m_linearVelocity.set( 50.0f, 50.0f );
		//cinfo.m_angularSpeed = 45.0f * g_degToRad;
		physicsBody* body = new physicsBody( cinfo );
		bIdB = world->addBody( body );
	}

	JointConfig config;
	config.pivot.setZero();
	config.bodyIdA = bIdA;
	config.bodyIdB = bIdB;
	world->addJoint( config );
#endif
#if 0
	{
		physicsBodyCinfo cinfo;
		vector<Vector3> vertices;
		vertices.push_back( Vector3( 20.0f, -40.0f ) );
		vertices.push_back( Vector3( 40.0f, -30.0f ) );
		vertices.push_back( Vector3( 50.0f, -20.0f ) );
		vertices.push_back( Vector3( 50.0f, 10.0f ) );
		vertices.push_back( Vector3( 20.0f, 40.0f ) );
		vertices.push_back( Vector3( -10.0f, 40.0f ) );
		vertices.push_back( Vector3( -40.0f, 10.0f ) );
		vertices.push_back( Vector3( -40.0f, -30.0f ) );
		vertices.push_back( Vector3( -30.0f, -40.0f ) );
		cinfo.m_shape = new physicsConvexShape( vertices, .1f );
		cinfo.m_pos.set( 115.0f, 65.0f );
		cinfo.m_ori = 77 * g_degToRad;
		//cinfo.m_angularSpeed = -50.0f * g_degToRad;
		physicsBody* body = new physicsBody( cinfo );
		world->addBody( body );
	}
#endif
#if 1
	{
		physicsBodyCinfo cinfo;
		vector<Vector3> vertices;
		vertices.push_back( Vector3( 40.0f, -40.0f ) );
		vertices.push_back( Vector3( 40.0f, 40.0f ) );
		vertices.push_back( Vector3( -40.0f, -40.0f ) );
		vertices.push_back( Vector3( -40.0f, 40.0f ) );
		cinfo.m_shape = new physicsConvexShape( vertices, .1f );
		cinfo.m_pos.set( 512.f, 384.f );
		//cinfo.m_ori = 77 * g_degToRad;
		//cinfo.m_angularSpeed = -50.0f * g_degToRad;
		physicsBody* body = new physicsBody( cinfo );
		world->addBody( body );
	}

#endif
#if 0
	{
		physicsBodyCinfo cinfo;
		vector<Vector3> vertices;
		vertices.push_back( Vector3( 20.0f, -40.0f ) );
		vertices.push_back( Vector3( 40.0f, -30.0f ) );
		vertices.push_back( Vector3( 50.0f, -20.0f ) );
		vertices.push_back( Vector3( 50.0f, 10.0f ) );
		vertices.push_back( Vector3( 20.0f, 40.0f ) );
		vertices.push_back( Vector3( -10.0f, 40.0f ) );
		vertices.push_back( Vector3( -40.0f, 10.0f ) );
		vertices.push_back( Vector3( -40.0f, -30.0f ) );
		vertices.push_back( Vector3( -30.0f, -40.0f ) );
		cinfo.m_shape = new physicsConvexShape( vertices, .1f );
		cinfo.m_pos.set( 115.0f, 70.0f );
		/// cinfo.m_rot = 77 * g_degToRad;
		cinfo.m_linearVelocity.set( -50.0f, -50.0f );
		cinfo.m_angularSpeed = -50.0f * g_degToRad;
		physicsBody* body = new physicsBody( cinfo );
		world->addBody( body );
	}
#endif
#if 0
	{
		physicsBodyCinfo cinfo;
		vector<Vector3> vertices;
		vertices.push_back( Vector3( 20.0f, -40.0f ) );
		vertices.push_back( Vector3( 40.0f, -30.0f ) );
		vertices.push_back( Vector3( 50.0f, -20.0f ) );
		vertices.push_back( Vector3( 50.0f, 10.0f ) );
		vertices.push_back( Vector3( 20.0f, 40.0f ) );
		vertices.push_back( Vector3( -10.0f, 40.0f ) );
		vertices.push_back( Vector3( -40.0f, 10.0f ) );
		vertices.push_back( Vector3( -40.0f, -30.0f ) );
		vertices.push_back( Vector3( -30.0f, -40.0f ) );
		cinfo.m_shape = new physicsConvexShape( vertices, .1f );
		cinfo.m_pos.set( 115.0f, 70.0f );
		/// cinfo.m_rot = 77 * g_degToRad;
		cinfo.m_linearVelocity.set( -50.0f, -50.0f );
		cinfo.m_angularSpeed = -50.0f * g_degToRad;
		physicsBody* body = new physicsBody( cinfo );
		world->addBody( body );
	}

	{
		physicsBodyCinfo cinfo;
		vector<Vector3> vertices;
		vertices.push_back( Vector3( 20.0f, -40.0f ) );
		vertices.push_back( Vector3( 40.0f, -30.0f ) );
		vertices.push_back( Vector3( 50.0f, -20.0f ) );
		vertices.push_back( Vector3( 50.0f, 10.0f ) );
		vertices.push_back( Vector3( 20.0f, 40.0f ) );
		vertices.push_back( Vector3( -10.0f, 40.0f ) );
		vertices.push_back( Vector3( -40.0f, 10.0f ) );
		vertices.push_back( Vector3( -40.0f, -30.0f ) );
		vertices.push_back( Vector3( -30.0f, -40.0f ) );
		cinfo.m_shape = new physicsConvexShape( vertices, .1f );
		cinfo.m_pos.set( 115.0f, 73.0f );
		/// cinfo.m_rot = 77 * g_degToRad;
		cinfo.m_linearVelocity.set( -50.0f, -50.0f );
		cinfo.m_angularSpeed = -50.0f * g_degToRad;
		physicsBody* body = new physicsBody( cinfo );
		world->addBody( body );
	}

	{
		physicsBodyCinfo cinfo;
		vector<Vector3> vertices;
		vertices.push_back( Vector3( 20.0f, -40.0f ) );
		vertices.push_back( Vector3( 40.0f, -30.0f ) );
		vertices.push_back( Vector3( 50.0f, -20.0f ) );
		vertices.push_back( Vector3( 50.0f, 10.0f ) );
		vertices.push_back( Vector3( 20.0f, 40.0f ) );
		vertices.push_back( Vector3( -10.0f, 40.0f ) );
		vertices.push_back( Vector3( -40.0f, 10.0f ) );
		vertices.push_back( Vector3( -40.0f, -30.0f ) );
		vertices.push_back( Vector3( -30.0f, -40.0f ) );
		cinfo.m_shape = new physicsConvexShape( vertices, .1f );
		cinfo.m_pos.set( 115.0f, 74.0f );
		/// cinfo.m_rot = 77 * g_degToRad;
		cinfo.m_linearVelocity.set( -50.0f, -50.0f );
		cinfo.m_angularSpeed = -50.0f * g_degToRad;
		physicsBody* body = new physicsBody( cinfo );
		world->addBody( body );
	}

	{
		physicsBodyCinfo cinfo;
		vector<Vector3> vertices;
		vertices.push_back( Vector3( 20.0f, -40.0f ) );
		vertices.push_back( Vector3( 40.0f, -30.0f ) );
		vertices.push_back( Vector3( 50.0f, -20.0f ) );
		vertices.push_back( Vector3( 50.0f, 10.0f ) );
		vertices.push_back( Vector3( 20.0f, 40.0f ) );
		vertices.push_back( Vector3( -10.0f, 40.0f ) );
		vertices.push_back( Vector3( -40.0f, 10.0f ) );
		vertices.push_back( Vector3( -40.0f, -30.0f ) );
		vertices.push_back( Vector3( -30.0f, -40.0f ) );
		cinfo.m_shape = new physicsConvexShape( vertices, .1f );
		cinfo.m_pos.set( 115.0f, 75.0f );
		/// cinfo.m_rot = 77 * g_degToRad;
		cinfo.m_linearVelocity.set( -50.0f, -50.0f );
		cinfo.m_angularSpeed = -50.0f * g_degToRad;
		physicsBody* body = new physicsBody( cinfo );
		world->addBody( body );
	}

#endif
#endif

#if 1
	{
		/// Eastern wall
		physicsBodyCinfo cinfo;
		vector<Vector3> vertices;
		vertices.push_back( Vector3( 25.f, 200.f ) );
		vertices.push_back( Vector3( -25.f, 250.f ) );
		vertices.push_back( Vector3( -25.f, -250.f ) );
		vertices.push_back( Vector3( 25.f, -200.f ) );
		cinfo.m_shape = new physicsConvexShape( vertices, .1f );
		cinfo.m_type = physicsMotionType::STATIC;
		cinfo.m_pos.set( 87.f, 384.f );
		physicsBody* body = new physicsBody( cinfo );
		world->addBody( body );
	}
#endif
#if 1
	{
		/// Northern wall
		physicsBodyCinfo cinfo;
		vector<Vector3> vertices;
		vertices.push_back( Vector3( -400.f, -25.f ) );
		vertices.push_back( Vector3( -450.f, 25.f ) );
		vertices.push_back( Vector3( 450.f, 25.f ) );
		vertices.push_back( Vector3( 400.f, -25.f ) );
		cinfo.m_shape = new physicsConvexShape( vertices, .1f );
		cinfo.m_type = physicsMotionType::STATIC;
		cinfo.m_pos.set( 512.f, 609.f );
		physicsBody* body = new physicsBody( cinfo );
		world->addBody( body );
	}
#endif
#if 1
	{
		/// Western wall
		physicsBodyCinfo cinfo;
		vector<Vector3> vertices;
		vertices.push_back( Vector3( -25.f, 200.f ) );
		vertices.push_back( Vector3( 25.f, 250.f ) );
		vertices.push_back( Vector3( 25.f, -250.f ) );
		vertices.push_back( Vector3( -25.f, -200.f ) );
		cinfo.m_shape = new physicsConvexShape( vertices, .1f );
		cinfo.m_type = physicsMotionType::STATIC;
		cinfo.m_pos.set( 937.f, 384.f );
		physicsBody* body = new physicsBody( cinfo );
		world->addBody( body );
	}
#endif
#if 1
	{
		/// Southern wall
		physicsBodyCinfo cinfo;
		vector<Vector3> vertices;
		vertices.push_back( Vector3( -400.f, 25.f ) );
		vertices.push_back( Vector3( -450.f, -25.f ) );
		vertices.push_back( Vector3( 450.f, -25.f ) );
		vertices.push_back( Vector3( 400.f, 25.f ) );
		cinfo.m_shape = new physicsConvexShape( vertices, .1f );
		cinfo.m_type = physicsMotionType::STATIC;
		cinfo.m_pos.set( 512.f, 159.f );
		physicsBody* body = new physicsBody( cinfo );
		world->addBody( body );
	}
#endif

	return 0;
}

int main( int argc, char* argv[] )
{
	Simulation( argc, argv );

	WindowCinfo cinfo;
	{
		cinfo.widthWindow = 1024;
		cinfo.heightWindow = 768;
		cinfo.framesPerSecond = 30;
		cinfo.world = world;
	}

	BeginGraphics( cinfo );
	return 0;
}
