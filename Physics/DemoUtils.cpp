#include <Base.hpp>
#include <DemoUtils.hpp>

#include <physicsBody.hpp>
#include <physicsShape.hpp>
#include <physicsWorld.hpp>

void DemoUtils::controlBody(ControlInfo& controlInfo, physicsWorld* world, BodyId bodyId, const Vector3 pos)
{
	// Grab bodies by attaching a dummy body to it using a joint constraint
	if (controlInfo.dummyBodyId == invalidId &&
		controlInfo.dummyJointId == invalidId)
	{
		physicsBodyCinfo cinfo;
		{
			cinfo.m_shape = physicsCircleShape::create(0.f);
			cinfo.m_type = physicsMotionType::STATIC;
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

		controlInfo.dummyJointId = world->addJoint(config);
	}

	world->setPosition(controlInfo.dummyBodyId, pos);
}

void DemoUtils::releaseControl(ControlInfo& controlInfo, physicsWorld* world, BodyId bodyId)
{
	world->removeBody(controlInfo.dummyBodyId);
	world->removeJoint(controlInfo.dummyJointId);

	controlInfo.dummyBodyId = invalidId;
	controlInfo.dummyJointId = invalidId;
}

void DemoUtils::createPackedCircles(std::vector<physicsBody>& bodies, int numCircles)
{
	Real radiusCircle = 5.0f;

	Vector3 arm(0.0f, -981.0f);
	Real radius = 0.0f;
	Real angle = 0.0f;
	int limitLayer = 1;

	for (int i = 0, j = 0; i < numCircles; i++, j++)
	{
		if (j == limitLayer)
		{
			radius += 2.f * radiusCircle;
			limitLayer = (int)(radius * (Real)M_PI / radiusCircle);
			angle = (360.f / limitLayer) * g_degToRad;
			arm.set(radius, 0.f);
			j = 0;
		}

		physicsBodyCinfo cinfo;
		cinfo.m_shape = physicsCircleShape::create(radiusCircle);

		Vector3 buf; buf.setRotatedDir(arm, angle*j);

		cinfo.m_pos = buf;
		bodies.push_back(physicsBody(cinfo));
	}
}
