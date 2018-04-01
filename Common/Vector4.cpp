#include <Common/Base.h>

#include <algorithm>

//
// Point class non-inline functions
Vector4::Vector4()
{
	setAll( 0.f );
}

Vector4::Vector4( const Real x, const Real y, const Real z )
{
	set( x, y, z );
}

Vector4::Vector4( const Vector4& copy )
{
	*this = copy;
}

void Vector4::setTransformedPos( const Transform& t, const Vector4& v )
{
	// Homogeneous coordinates for translation
	Vector4 vh( v ); vh( 2 ) = 1.f;

	( *this )( 0 ) = t( 0, 0 )*vh( 0 ) + t( 0, 1 )*vh( 1 ) + t( 0, 2 )*vh( 2 );
	( *this )( 1 ) = t( 1, 0 )*vh( 0 ) + t( 1, 1 )*vh( 1 ) + t( 1, 2 )*vh( 2 );
}

void Vector4::setTransformedInversePos( const Transform& t, const Vector4& v )
{
	Transform tinv; tinv.setInverse( t );
	setTransformedPos( tinv, v );
}

void Vector4::setClampedLength( const Vector4& v, const Real length )
{
	Assert( !v.isZero(), "Trying to give length to zero vector" );
	Assert( length > std::numeric_limits<Real>::epsilon(), "Trying to scale to negative length" );
	Real scale = length / v.length<2>();
	scale = std::min( 1.f, scale );
	( *this )( 0 ) = v( 0 ) * scale;
	( *this )( 1 ) = v( 1 ) * scale;
	( *this )( 2 ) = v( 2 ) * scale;
}

std::ostream& operator<<( std::ostream& stream, Vector4& v )
{
	stream.precision( 2 );
	stream << std::fixed << v( 0 ) << "," << v( 1 ) << std::endl;
	return stream;
}