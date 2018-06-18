#pragma once

class physicsBoxShape;
class Vector4;

namespace physicsShapeUtils
{
	// Determine closest edge of box closest to point
	void getClosestEdgeToPoint( const physicsBoxShape& box, 
								const Vector4& point,
								Vector4& base, 
								Vector4& edge );
}