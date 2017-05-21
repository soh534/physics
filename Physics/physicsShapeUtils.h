#pragma once

class physicsBoxShape;
class Vector3;

namespace physicsShapeUtils
{
	/// Determine closest edge of box closest to point
	void getClosestEdgeToPoint( const physicsBoxShape& box, 
								const Vector3& point,
								Vector3& base, 
								Vector3& edge );
}