#pragma once

class physicsBody;
class physicsWorld;

#include <physicsTypes.hpp>

namespace DemoUtils
{
	struct ControlInfo
	{
		BodyId dummyBodyId;
		JointId dummyJointId;

		ControlInfo()
		{
			dummyBodyId = -1;
			dummyJointId = -1;
		}
	};

	// Allow key-framed control of bodies in world
	void controlBody(ControlInfo& controlInfo, physicsWorld* world, BodyId bodyId, const Point3 pos);
	void releaseControl(ControlInfo& controlInfo, physicsWorld* world, BodyId bodyId);

	// Create packed circles for mass simulation
	void createPackedCircles(std::vector<physicsBody*>& bodies, int numCircles);
}
