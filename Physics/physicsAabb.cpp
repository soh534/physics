#include <physicsAabb.h>

#include <cstdlib> // abs()

physicsAabb::physicsAabb()
{
	m_max.setAll( 0.f );
	m_min.setAll( 0.f );
}

physicsAabb::physicsAabb( Vector3 max, Vector3 min )
	: m_max( max ), m_min( min )
{
}

void physicsAabb::includeAabb( const physicsAabb& aabb )
{
	if ( m_max < aabb.m_max )
	{
		m_max = aabb.m_max;
	}
	if ( m_min > aabb.m_min )
	{
		m_min = aabb.m_min;
	}
}

bool physicsAabb::overlaps( const physicsAabb& aabb )
{
	Vector3 centerA; centerA.setAdd( m_max, m_min );
	centerA.setMul( centerA, 0.5f );

	Vector3 centerB; centerB.setAdd( aabb.m_max, aabb.m_min );
	centerB.setMul( centerB, 0.5f );

	Real dx = abs( centerA( 0 ) - centerB( 0 ) );
	Real dy = abs( centerA( 1 ) - centerB( 1 ) );

	Real sumX = m_max( 0 ) - centerA( 0 ) + aabb.m_max( 0 ) - centerB( 0 );
	Real sumY = m_max( 1 ) - centerA( 1 ) + aabb.m_max( 1 ) - centerB( 1 );

	return ( dx < sumX && dy < sumY );
}

void physicsAabb::expand( const Real factor )
{
	m_max.setMul( m_max, 1.f + factor );
	m_min.setMul( m_min, 1.f + factor );
}

void physicsAabb::expand( const Vector3 & direction )
{
	Vector3 newMax = m_max + direction;
	Vector3 newMin = m_min + direction;
	m_max.setMax( newMax );
	m_min.setMin( newMin );
}

void physicsAabb::translate( const Vector3& translation )
{
	m_max.setAdd( m_max, translation );
	m_min.setAdd( m_min, translation );
}
