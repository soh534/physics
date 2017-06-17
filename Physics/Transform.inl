#include "Transform.h"

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

inline void Transform::setTranspose( const Transform& m )
{
	Assert( this != &m, "feeding self as input" );

	for ( unsigned int i = 0; i < 3; i++ )
	{
		for ( unsigned int j = 0; j < 3; j++ )
		{
			( *this )( j, i ) = m( i, j );
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
	Real rot00, rot01, rot10, rot11;
	rot00 = acos( m_data[0][0] );
	rot01 = asin( -m_data[0][1] );
	rot10 = asin( m_data[1][0] );
	rot11 = acos( m_data[1][1] );

	Assert( rot00 == rot01, "faulty getRotation()" );
	Assert( rot10 == rot11, "faulty getRotation()" );
	Assert( rot01 == rot10, "faulty getRotation()" );

	return Real();
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
	const Real xsq = direction( 0 ) * direction( 0 );
	const Real ysq = direction( 1 ) * direction( 1 );
	m_data[0][0] = xsq - ysq;
	m_data[0][1] = 2.f * direction( 0 ) * direction( 1 );
	m_data[1][0] = 2.f * direction( 0 ) * direction( 1 );
	m_data[1][1] = ysq - xsq;
}

/// TO-DO - this function doesn't work, repair it
inline void Transform::setMul( const Transform& m0, const Transform& m1 )
{
	Assert( this != &m0 && this != &m1, "Feeding self as input" );
	setZero();

	for ( int i = 0; i < 3; i++ )
	{
		for ( int j = 0; j < 3; j++ )
		{
			for ( int k = 0; k < 3; k++ )
			{
				m_data[i][j] += m0( i, k ) * m1( k, j );
			}
		}
	}
}

inline void Transform::mul( const Transform& t )
{
	Assert( false, "Function is not implemented yet" );
}

inline void Transform::setAdd( const Transform& m0, const Transform& m1 )
{
	for ( int i = 0; i < 3; i++ )
	{
		for ( int j = 0; j < 3; j++ )
		{
			m_data[i][j] = m0( i, j ) + m1( i, j );
		}
	}
}

inline void Transform::setSub( const Transform& m0, const Transform& m1 )
{
	for ( int i = 0; i < 3; i++ )
	{
		for ( int j = 0; j < 3; j++ )
		{
			m_data[i][j] = m0( i, j ) - m1( i, j );
		}
	}
}

inline void Transform::setInverse( const Transform& m )
{
	Assert( this != &m, "Feeding self as input" );
	setIdentity();

	for ( int i = 0; i < 2; i++ )
	{
		for ( int j = 0; j < 2; j++ )
		{
			m_data[i][j] = m.m_data[j][i];
		}
	}

	m_data[0][2] = -1.f * ( m_data[0][0] * m.m_data[0][2] + m_data[0][1] * m.m_data[1][2] );
	m_data[1][2] = -1.f * ( m_data[1][0] * m.m_data[0][2] + m_data[1][1] * m.m_data[1][2] );
}
