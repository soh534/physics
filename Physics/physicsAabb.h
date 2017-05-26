#pragma once

#include <Base.h>

class physicsAabb
{
public:

	physicsAabb();
	physicsAabb( Vector3 max, Vector3 min );
	void includeAabb( const physicsAabb& aabb );
	bool overlaps( const physicsAabb& aabb );
	void expand( const Real factor );
	void translate( const Vector3& translation );

	Vector3 m_max;
	Vector3 m_min;
};
