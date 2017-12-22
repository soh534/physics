#pragma once

#include <vector>
#include <physicsTypes.h>

class physicsBody;
class physicsWorld;

namespace DemoUtils
{
	struct ControlInfo
	{
		BodyId dummyBodyId;
		JointId dummyJointId;

		ControlInfo()
		{
			dummyBodyId = invalidId;
			dummyJointId = invalidId;
		}
	};

	// Allow mouse-picked control of bodies in world
	void grab( ControlInfo& controlInfo, physicsWorld* world, BodyId bodyId, const Vector3 pos );
	void release( ControlInfo& controlInfo, physicsWorld* world, BodyId bodyId );

	// Create packed circles for mass simulation
	void createPackedCircles( physicsWorld* world, const Vector3& pos, const Real radius, const int numCircles );
}
