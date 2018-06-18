#include <Base.h>
#include <DemoUtils.h>
#include <physicsShape.h>
#include <physicsWorld.h>
#include <vector>

void DemoUtils::grab( ControlInfo& controlInfo, std::shared_ptr<physicsWorld>& world, BodyId bodyId, const Vector4& pos )
{
	// Grab bodies by attaching a dummy body to it using a joint constraint
	if ( controlInfo.dummyBodyId == invalidId &&
		 controlInfo.dummyJointId == invalidId )
	{
		physicsBodyCinfo cinfo;
		{
			cinfo.m_shape = physicsCircleShape::create( 0.f );
			cinfo.m_motionType = physicsMotionType::STATIC;
			cinfo.m_pos = pos;
			cinfo.m_collidable = false;
		}

		controlInfo.dummyBodyId = world->createBody( cinfo );

		JointConfig config;
		{
			config.pivot = pos;
			config.bodyIdA = bodyId;
			config.bodyIdB = controlInfo.dummyBodyId;
		}

		controlInfo.dummyJointId = world->addJoint( config );
	}

	world->setPosition( controlInfo.dummyBodyId, pos );
}

void DemoUtils::release( ControlInfo& controlInfo, std::shared_ptr<physicsWorld>& world, BodyId bodyId )
{
	world->removeBody( controlInfo.dummyBodyId );
	world->removeJoint( controlInfo.dummyJointId );

	controlInfo.dummyBodyId = invalidId;
	controlInfo.dummyJointId = invalidId;
}

void DemoUtils::createPackedCircles( std::shared_ptr<physicsWorld>& world,
									 const Vector4& pos,
									 const Real radius,
									 const int numCircles )
{
	Vector4 arm( 0.0f, 0.0f );
	Real armRadius = 0.0f;
	Real angle = 0.0f;
	int limitLayer = 1;

	for ( int i = 0, j = 0; i < numCircles; i++, j++ )
	{
		if ( j == limitLayer )
		{
			armRadius += 2.f * radius;
			limitLayer = ( int )( ( Real )M_PI * armRadius ) / radius;
			angle = ( 360.f / limitLayer ) * g_degToRad;
			arm.set( armRadius, 0.f );
			j = 0;
		}

		physicsBodyCinfo cinfo;
		{
			cinfo.m_shape = physicsCircleShape::create( radius );
			Vector4 buf; buf.setRotatedDir( arm, angle*j );
			cinfo.m_pos = buf + pos;
		}

		world->createBody( cinfo );
	}
}

void DemoUtils::createWalls( std::shared_ptr<physicsWorld>& world )
{
#if 1
	{
		// Eastern wall
		physicsBodyCinfo cinfo;
		std::vector<Vector4> vertices;
		vertices.push_back( Vector4( 25.f, 200.f ) );
		vertices.push_back( Vector4( -25.f, 250.f ) );
		vertices.push_back( Vector4( -25.f, -250.f ) );
		vertices.push_back( Vector4( 25.f, -200.f ) );
		cinfo.m_shape = physicsConvexShape::create( vertices, .1f );
		cinfo.m_motionType = physicsMotionType::STATIC;
		cinfo.m_pos.set( 87.f, 384.f );
		world->createBody( cinfo );
	}
#endif
#if 1
	{
		// Northern wall
		physicsBodyCinfo cinfo;
		std::vector<Vector4> vertices;
		vertices.push_back( Vector4( -400.f, -25.f ) );
		vertices.push_back( Vector4( -450.f, 25.f ) );
		vertices.push_back( Vector4( 450.f, 25.f ) );
		vertices.push_back( Vector4( 400.f, -25.f ) );
		cinfo.m_shape = physicsConvexShape::create( vertices, .1f );
		cinfo.m_motionType = physicsMotionType::STATIC;
		cinfo.m_pos.set( 512.f, 609.f );
		world->createBody( cinfo );
	}
#endif
#if 1
	{
		// Western wall
		physicsBodyCinfo cinfo;
		std::vector<Vector4> vertices;
		vertices.push_back( Vector4( -25.f, 200.f ) );
		vertices.push_back( Vector4( 25.f, 250.f ) );
		vertices.push_back( Vector4( 25.f, -250.f ) );
		vertices.push_back( Vector4( -25.f, -200.f ) );
		cinfo.m_shape = physicsConvexShape::create( vertices, .1f );
		cinfo.m_motionType = physicsMotionType::STATIC;
		cinfo.m_pos.set( 937.f, 384.f );
		world->createBody( cinfo );
	}
#endif
#if 1
	{
		// Southern wall
		physicsBodyCinfo cinfo;
		std::vector<Vector4> vertices;
		vertices.push_back( Vector4( -400.f, 25.f ) );
		vertices.push_back( Vector4( -450.f, -25.f ) );
		vertices.push_back( Vector4( 450.f, -25.f ) );
		vertices.push_back( Vector4( 400.f, 25.f ) );
		cinfo.m_shape = physicsConvexShape::create( vertices, .1f );
		cinfo.m_motionType = physicsMotionType::STATIC;
		cinfo.m_pos.set( 512.f, 159.f );
		world->createBody( cinfo );
	}
#endif
}

void DemoUtils::createConstrainedBodies( std::shared_ptr<physicsWorld>& world )
{
	int bIdA, bIdB;
	{
		physicsBodyCinfo cinfo;
		std::vector<Vector4> vertices;
		vertices.push_back( Vector4( -18.0f, 36.0f ) );
		vertices.push_back( Vector4( -30.0f, 6.0f ) );
		vertices.push_back( Vector4( 42.0, -6.0 ) );
		vertices.push_back( Vector4( -30.0f, -30.0f ) );
		vertices.push_back( Vector4( 12.0f, -60.0f ) );
		vertices.push_back( Vector4( 30.0f, 18.0f ) );
		cinfo.m_shape = physicsConvexShape::create( vertices, .1f );
		cinfo.m_pos.set( 462.0f, 334.0f );
		// cinfo.m_rot = 45.0f * g_degToRad;
		cinfo.m_linearVelocity.set( 50.0f, 50.0f );
		//cinfo.m_angularSpeed = 45.0f * g_degToRad;
		bIdA = world->createBody( cinfo );
	}

	{
		physicsBodyCinfo cinfo;
		std::vector<Vector4> vertices;
		vertices.push_back( Vector4( -18.0f, 36.0f ) );
		vertices.push_back( Vector4( -30.0f, 6.0f ) );
		vertices.push_back( Vector4( 42.0, -6.0 ) );
		vertices.push_back( Vector4( -30.0f, -30.0f ) );
		vertices.push_back( Vector4( 12.0f, -60.0f ) );
		vertices.push_back( Vector4( 30.0f, 18.0f ) );
		cinfo.m_shape = physicsConvexShape::create( vertices, .1f );
		cinfo.m_pos.set( 562.0f, 434.0f );
		// cinfo.m_rot = 45.0f * g_degToRad;
		cinfo.m_linearVelocity.set( 50.0f, 50.0f );
		//cinfo.m_angularSpeed = 45.0f * g_degToRad;
		bIdB = world->createBody( cinfo );
	}

	JointConfig config;
	config.pivot.set( 512.f, 384.f );
	config.bodyIdA = bIdA;
	config.bodyIdB = bIdB;
	world->addJoint( config );
}