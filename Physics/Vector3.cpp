#include <Base.h>

#include <algorithm>

///
/// Point class non-inline functions
Vector3::Vector3()
	: x( 0.f ), y( 0.f ), z( 0.f )
{

}

Vector3::Vector3( const Real x, const Real y, const Real z )
	: x( x ), y( y ), z( z )
{

}

Vector3::Vector3( const Vector3& copy )
	: x( copy.x ), y( copy.y ), z( copy.z )
{
	/// Over-ride copy constructor
}

void Vector3::setTransformedPos( const Transform& t, const Vector3& v )
{
	Vector3 vh( v ); vh( 2 ) = 1.f; /// Add additional dimension to enable translation
	x = t( 0, 0 )*vh( 0 ) + t( 0, 1 )*vh( 1 ) + t( 0, 2 )*vh( 2 );
	y = t( 1, 0 )*vh( 0 ) + t( 1, 1 )*vh( 1 ) + t( 1, 2 )*vh( 2 );
}

void Vector3::setTransformedInversePos( const Transform& t, const Vector3& v )
{
	Transform tinv; tinv.setInverse( t );
	return setTransformedPos( tinv, v );
}

void Vector3::setClampedLength( const Vector3& v, const Real length )
{
	Assert( !v.isZero(), "Trying to give length to zero vector" );
	Assert( length > std::numeric_limits<Real>::epsilon(), "Trying to scale to negative length" );
	Real scale = length / v.length();
	scale = std::min( 1.f, scale );
	x = v.x * scale;
	y = v.y * scale;
	z = v.z * scale;
}

std::ostream& operator<<( std::ostream& stream, Vector3& v )
{
	stream.precision( 2 );
	stream << std::fixed << v( 0 ) << "," << v( 1 ) << std::endl;
	return stream;
}