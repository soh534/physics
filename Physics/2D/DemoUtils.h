#pragma once

#include <memory>            // std::shared_ptr
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
	void grab( ControlInfo& controlInfo, std::shared_ptr<physicsWorld>& world, BodyId bodyId, const Vector4& pos );
	void release( ControlInfo& controlInfo, std::shared_ptr<physicsWorld>& world, BodyId bodyId );

	// Creates packed circles, useful for testing
	void createPackedCircles(
		std::shared_ptr<physicsWorld>& world, const Vector4& pos, const Real radius, const int numCircles );

	// Creates static walls preventing inner bodies from getting out
	void createWalls( std::shared_ptr<physicsWorld>& world );

	// Adds constrained bodies, useful for testing constraints
	void createConstrainedBodies( std::shared_ptr<physicsWorld>& world );
}
