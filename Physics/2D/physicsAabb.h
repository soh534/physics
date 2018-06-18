#pragma once

#include <Base.h>

class physicsAabb
{
public:

	physicsAabb();
	physicsAabb( const Vector4& max, const Vector4& min );
	void includeAabb( const physicsAabb& aabb );
	bool overlaps( const physicsAabb& aabb );
	void expand( const Real factor );
	void expand( const Vector4& direction );
	void translate( const Vector4& translation );

	Vector4 m_max;
	Vector4 m_min;
};
