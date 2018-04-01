#include <Scene.h>

#include <physicsWorld.h>
#include <DemoUtils.h>

namespace Scenes
{
	int testScene( physicsWorld*& world )
	{
#if 0
		{
			physicsBodyCinfo cinfo;
			cinfo.m_shape = physicsBoxShape::create( Vector3( 10.f, 10.f ) );
			cinfo.m_pos.set( 250.0f, 200.0f );
			cinfo.m_ori = 30.0f * g_degToRad;
			cinfo.m_linearVelocity.set( -10.f, -10.f );
			world->createBody( cinfo );
		}

		{
			physicsBodyCinfo cinfo;
			cinfo.m_shape = physicsBoxShape::create( Vector3( 20.f, 30.f ) );
			cinfo.m_pos.set( 300.0f, 200.0f );
			cinfo.m_ori = 45.0f * g_degToRad;
			cinfo.m_linearVelocity.set( 20.f, 20.f );
			world->createBody( cinfo );
		}
#endif

#if 0
		{
			physicsBodyCinfo cinfo;
			cinfo.m_shape = new physicsCircleShape( 40.0f );
			cinfo.m_pos.set( 230.0f, 150.0f );
			cinfo.m_pos.set( -50.f, -50.f );
			cinfo.m_linearVelocity.set( 50.f, 50.f );
			// cinfo.m_angularSpeed = 50.f * g_degToRad;
			physicsBody* body = new physicsBody( cinfo );
			world->addBody( body );
		}

		{
			physicsBodyCinfo cinfo;
			cinfo.m_shape = new physicsCircleShape( 60.0f );
			cinfo.m_pos.set( -230.0f, -150.0f );
			cinfo.m_pos.set( 50.f, 50.f );
			cinfo.m_linearVelocity.set( -10.f, -10.f );
			// cinfo.m_angularSpeed = 100.f * g_degToRad;
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
			cinfo.m_shape = physicsConvexShape::create( vertices, .1f );
			cinfo.m_pos.set( 627.0f, 449.0f );
			cinfo.m_ori = 77 * g_degToRad;
			//cinfo.m_angularSpeed = -50.0f * g_degToRad;
			world->createBody( cinfo );
		}
#endif
#if 0
		{
			physicsBodyCinfo cinfo;
			vector<Vector3> vertices;
			vertices.push_back( Vector3( 40.0f, -40.0f ) );
			vertices.push_back( Vector3( 40.0f, 40.0f ) );
			vertices.push_back( Vector3( -40.0f, -40.0f ) );
			vertices.push_back( Vector3( -40.0f, 40.0f ) );
			cinfo.m_shape = physicsConvexShape::create( vertices, .1f );
			cinfo.m_pos.set( 512.f, 384.f );
			cinfo.m_ori = 77 * g_degToRad;
			//cinfo.m_linearVelocity.set( 50.f, 0. );
			//cinfo.m_angularSpeed = -50.0f * g_degToRad;
			world->createBody( cinfo );
		}
#endif
#if 0
		{
			physicsBodyCinfo cinfo;
			vector<Vector3> vertices;
			vertices.push_back( Vector3( 40.0f, -40.0f ) );
			vertices.push_back( Vector3( 40.0f, 40.0f ) );
			vertices.push_back( Vector3( -40.0f, -40.0f ) );
			vertices.push_back( Vector3( -40.0f, 40.0f ) );
			cinfo.m_shape = physicsConvexShape::create( vertices, .1f );
			cinfo.m_pos.set( 562.f, 384.f );
			cinfo.m_ori = 77 * g_degToRad;
			//cinfo.m_linearVelocity.set( 50.f, 0. );
			//cinfo.m_angularSpeed = -50.0f * g_degToRad;
			world->createBody( cinfo );
		}
#endif
#if 0
		{
			physicsBodyCinfo cinfo;
			vector<Vector3> vertices;
			vertices.push_back( Vector3( 40.0f, -40.0f ) );
			vertices.push_back( Vector3( 40.0f, 40.0f ) );
			vertices.push_back( Vector3( -40.0f, -40.0f ) );
			vertices.push_back( Vector3( -40.0f, 40.0f ) );
			cinfo.m_shape = physicsConvexShape::create( vertices, .1f );
			cinfo.m_pos.set( 450.f, 384.f );
			cinfo.m_ori = 77 * g_degToRad;
			//cinfo.m_linearVelocity.set( 50.f, 0. );
			//cinfo.m_angularSpeed = -50.0f * g_degToRad;
			world->createBody( cinfo );
		}
#endif
#if 0
		{
			physicsBodyCinfo info;
			info.m_shape = physicsCircleShape::create( 40.f );
			info.m_pos.set( 500.f, 500.f );
			world->createBody( info );
		}
#endif
#if 0
		{
			physicsBodyCinfo info;
			info.m_shape = physicsCircleShape::create( 40.f );
			info.m_pos.set( 600.f, 500.f );
			world->createBody( info );
		}
#endif
#if 0
		{
			physicsBodyCinfo cinfo;
			vector<Vector3> vertices;
			vertices.push_back( Vector3( -18.0f, 36.0f ) );
			vertices.push_back( Vector3( -30.0f, 6.0f ) );
			vertices.push_back( Vector3( 42.0, -6.0 ) );
			vertices.push_back( Vector3( -30.0f, -30.0f ) );
			vertices.push_back( Vector3( 12.0f, -60.0f ) );
			vertices.push_back( Vector3( 30.0f, 18.0f ) );
			cinfo.m_shape = physicsConvexShape::create( vertices, .1f );
			cinfo.m_pos.set( 462.0f, 434.0f );
			// cinfo.m_rot = 45.0f * g_degToRad;
			cinfo.m_linearVelocity.set( 50.0f, 50.0f );
			//cinfo.m_angularSpeed = 45.0f * g_degToRad;
			world->createBody( cinfo );
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
			// cinfo.m_rot = 77 * g_degToRad;
			cinfo.m_linearVelocity.set( -50.0f, -50.0f );
			cinfo.m_angularSpeed = -50.0f * g_degToRad;
			world->createBody( cinfo );
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
			// cinfo.m_rot = 77 * g_degToRad;
			cinfo.m_linearVelocity.set( -50.0f, -50.0f );
			cinfo.m_angularSpeed = -50.0f * g_degToRad;
			world->createBody( cinfo );
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
			// cinfo.m_rot = 77 * g_degToRad;
			cinfo.m_linearVelocity.set( -50.0f, -50.0f );
			cinfo.m_angularSpeed = -50.0f * g_degToRad;
			world->createBody( cinfo );
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
			// cinfo.m_rot = 77 * g_degToRad;
			cinfo.m_linearVelocity.set( -50.0f, -50.0f );
			cinfo.m_angularSpeed = -50.0f * g_degToRad;
			world->createBody( cinfo );
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
			// cinfo.m_rot = 77 * g_degToRad;
			cinfo.m_linearVelocity.set( -50.0f, -50.0f );
			cinfo.m_angularSpeed = -50.0f * g_degToRad;
			world->createBody( cinfo );
		}
#endif
		return 0;
	}

	std::shared_ptr<physicsWorld> oneConvexScene( const physicsWorldConfig& wcfg )
	{
		std::shared_ptr<physicsWorld> world( new physicsWorld( wcfg ) );

		{
			physicsBodyCinfo cinfo;
			std::vector<Vector4> vertices;
			vertices.push_back( Vector4( 20.0f, -40.0f ) );
			vertices.push_back( Vector4( 40.0f, -30.0f ) );
			vertices.push_back( Vector4( 50.0f, -20.0f ) );
			vertices.push_back( Vector4( 50.0f, 10.0f ) );
			vertices.push_back( Vector4( 20.0f, 40.0f ) );
			vertices.push_back( Vector4( -10.0f, 40.0f ) );
			vertices.push_back( Vector4( -40.0f, 10.0f ) );
			vertices.push_back( Vector4( -40.0f, -30.0f ) );
			vertices.push_back( Vector4( -30.0f, -40.0f ) );
			cinfo.m_shape = physicsConvexShape::create( vertices, .1f );
			cinfo.m_pos.set( 627.0f, 449.0f );
			cinfo.m_ori = 77 * g_degToRad;
			//cinfo.m_angularSpeed = -50.0f * g_degToRad;
			world->createBody( cinfo );
		}

		DemoUtils::createWalls( world );

		return world;
	}

	std::shared_ptr<physicsWorld> manyCirclesScene( const physicsWorldConfig& wcfg )
	{
		std::shared_ptr<physicsWorld> world( new physicsWorld( wcfg ) );

		DemoUtils::createPackedCircles( world, Vector4( 400.f, 400.f ), 50.f, 30 );
		DemoUtils::createWalls( world );

		return world;
	}

	std::shared_ptr<physicsWorld> massParticlesScene( const physicsWorldConfig& wcfg )
	{
		std::shared_ptr<physicsWorld> world( new physicsWorld( wcfg ) );

		DemoUtils::createPackedCircles( world, Vector4( 400.f, 400.f ), 10.f, 100 );
		DemoUtils::createWalls( world );

		return world;
	}
}

std::shared_ptr<physicsWorld> initPhysicsScene()
{
	physicsWorldConfig wcfg;
	//cinfo.m_gravity.setZero();
	wcfg.m_gravity.set( 0.f, -981.f );
	wcfg.m_numIter = 4;

	//return Scenes::oneConvexScene( wcfg );
	//return Scenes::manyCirclesScene( wcfg );
	return Scenes::massParticlesScene( wcfg );
}
