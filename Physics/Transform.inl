#include "Transform.h"

#include <cmath>

inline const Real& Transform::operator()( int i, int j ) const
{
	return m_data[i][j];
}

inline Real& Transform::operator()( int i, int j )
{
	return m_data[i][j];
}

inline void Transform::setIdentity()
{
	for ( int i = 0; i < 3; i++ )
	{
		for ( int j = 0; j < 3; j++ )
		{
			if ( i == j )
			{
				m_data[i][j] = 1.f;
			}
			else
			{
				m_data[i][j] = 0.f;
			}
		}
	}
}

inline void Transform::setZero()
{
	for ( int i = 0; i < 3; i++ )
	{
		for ( int j = 0; j < 3; j++ )
		{
			m_data[i][j] = 0.f;
		}
	}
}

inline void Transform::addTranslation( const Vector3& translation )
{
	m_data[0][2] = translation( 0 );
	m_data[1][2] = translation( 1 );
}

inline void Transform::setTranslation( const Vector3& translation )
{
	setIdentity();
	addTranslation( translation );
}

inline Vector3 Transform::getTranslation() const
{
	return Vector3( m_data[0][2], m_data[1][2] );
}

inline void Transform::addRotation( const Real rotation )
{
	m_data[0][0] = cos( rotation ); m_data[0][1] = -sin( rotation );
	m_data[1][0] = sin( rotation ); m_data[1][1] = cos( rotation );
}

inline void Transform::setRotation( const Real rotation )
{
	setIdentity();
	addRotation( rotation );
}

inline Real Transform::getRotation() const
{
	Real acos00 = acos( m_data[0][0] );
	Real asin01 = asin( -m_data[0][1] );
	Real asin10 = asin( m_data[1][0] );
	Real acos11 = acos( m_data[1][1] );

	Assert( acos00 == acos11, "faulty getRotation()" );
	Assert( asin01 == asin10, "faulty getRotation()" );

	return asin01;
}

inline void Transform::setTransform( const Vector3& translation, const Real rotation )
{
	setIdentity();
	addTranslation( translation );
	addRotation( rotation );
}

inline void Transform::setReflection( const Vector3& direction )
{
	setIdentity();
	const Real xx = direction( 0 ) * direction( 0 );
	const Real yy = direction( 1 ) * direction( 1 );
	m_data[0][0] = xx - yy;
	m_data[0][1] = 2.f * direction( 0 ) * direction( 1 );
	m_data[1][0] = 2.f * direction( 0 ) * direction( 1 );
	m_data[1][1] = yy - xx;
}

inline void Transform::setMul( const Transform& t0, const Transform& t1 )
{
	Transform t0copy( t0 );
	Transform t1copy( t1 );

	setZero();

	for ( int i = 0; i < 3; i++ )
	{
		for ( int j = 0; j < 3; j++ )
		{
			for ( int k = 0; k < 3; k++ )
			{
				m_data[i][j] += t0copy( i, k ) * t1copy( k, j );
			}
		}
	}
}

inline void Transform::mul( const Transform& t )
{
	setMul( *this, t );
}

inline void Transform::setInverse( const Transform& t )
{
	/// Copy
	Transform copy( t );

	setIdentity();

	for ( int i = 0; i < 2; i++ )
	{
		for ( int j = 0; j < 2; j++ )
		{
			m_data[i][j] = copy.m_data[j][i];
		}
	}

	m_data[0][2] = -1.f * ( m_data[0][0] * copy.m_data[0][2] + m_data[0][1] * copy.m_data[1][2] );
	m_data[1][2] = -1.f * ( m_data[1][0] * copy.m_data[0][2] + m_data[1][1] * copy.m_data[1][2] );
}

inline void Transform::invert()
{
	setInverse( *this );
}

inline bool Transform::isApproximatelyEqual( const Transform& t, Real epsilon)
{
	bool res = true;

	for ( int i = 0; i < 3; i++ )
	{
		for ( int j = 0; j < 3; j++ )
		{
			if ( fabs( m_data[i][j] - t.m_data[i][j] ) > epsilon )
			{
				res = false;
				break;
			}
		}
	}

	return res;
}