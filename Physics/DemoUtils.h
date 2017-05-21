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
			dummyBodyId = -1;
			dummyJointId = -1;
		}
	};

	/// Allow key-framed control of bodies in world
	void controlBody( ControlInfo& controlInfo, physicsWorld* world, BodyId bodyId, const Vector3 pos );
	void releaseControl( ControlInfo& controlInfo, physicsWorld* world, BodyId bodyId );

	/// Create packed circles for mass simulation
	void createPackedCircles( std::vector<physicsBody>& bodies, int numCircles );
}
