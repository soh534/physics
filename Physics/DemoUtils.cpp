#include <Base.h>
#include <DemoUtils.h>
#include <physicsShape.h>
#include <physicsWorld.h>

void DemoUtils::controlBody( ControlInfo& controlInfo, physicsWorld* world, BodyId bodyId, const Vector3 pos )
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

		JointCinfo config;
		{
			config.pivot = pos;
			config.bodyIdA = bodyId;
			config.bodyIdB = controlInfo.dummyBodyId;
		}

		controlInfo.dummyJointId = world->addJoint( config );
	}

	world->setPosition( controlInfo.dummyBodyId, pos );
}

void DemoUtils::releaseControl( ControlInfo& controlInfo, physicsWorld* world, BodyId bodyId )
{
	world->removeBody( controlInfo.dummyBodyId );
	world->removeJoint( controlInfo.dummyJointId );

	controlInfo.dummyBodyId = invalidId;
	controlInfo.dummyJointId = invalidId;
}

void DemoUtils::createPackedCircles( physicsWorld* world,
									 const Vector3& pos,
									 const Real radius,
									 const int numCircles )
{
	Vector3 arm( 0.0f, 0.0f );
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
			Vector3 buf; buf.setRotatedDir( arm, angle*j );
			cinfo.m_pos = buf + pos;
		}

		world->createBody( cinfo );
	}
}
