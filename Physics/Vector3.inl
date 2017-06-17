#define _USE_MATH_DEFINES
#include <math.h> /// For M_PI definition
#include <cmath> /// For sqrt, cos, sin
#include <cfloat> /// For FLT_EPSILON
#include <algorithm> /// For max

#include <Base.h>
#include "Vector3.h"

///
/// Setting
inline void Vector3::operator=( const Vector3& other )
{
	x = other.x;
	y = other.y;
	z = other.z;
}

inline void Vector3::set( const Real a, const Real b, const Real c )
{
	x = a;
	y = b;
	z = c;
}

inline void Vector3::setAll( const Real val )
{
	x = val;
	y = val;
	z = val;;
}

///
/// Getting
inline const Real& Vector3::operator()( int i ) const
{
	Assert( i == 0 || i == 1 || i == 2, "Looking up invalid index." );

	if ( i == 0 )
	{
		return x;
	}
	else if ( i == 1 )
	{
		return y;
	}
	else
	{
		return z;
	}
}

/// TODO: Duplicate code...
inline Real& Vector3::operator()( int i )
{
	Assert( i == 0 || i == 1 || i == 2, "Looking up invalid index." );

	if ( i == 0 )
	{
		return x;
	}
	else if ( i == 1 )
	{
		return y;
	}
	else
	{
		return z;
	}
}

///
/// Allow construction and returns
inline const Vector3 Vector3::operator+( const Vector3& other ) const
{
	return Vector3( x + other.x, y + other.y, z + other.z );
}

inline void Vector3::operator+=( const Vector3& rhs )
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
}

inline const Vector3 Vector3::operator-( const Vector3& other ) const
{
	return Vector3( x - other.x, y - other.y, z - other.z );
}

inline void Vector3::operator-=( const Vector3& rhs )
{
	x -= rhs.x;
	y -= rhs.y;
	z -= rhs.z;
}

inline const Vector3 Vector3::operator*( const Vector3& other ) const
{
	return Vector3( x * other.x, y * other.y, z * other.z );
}

inline const Vector3 Vector3::operator*( const Real scale ) const
{
	return Vector3( x * scale, y * scale, z * scale );
}

inline void Vector3::operator*=( const Vector3& rhs )
{
	x *= rhs.x;
	y *= rhs.y;
	z *= rhs.z;
}

inline void Vector3::operator*=( const Real scale )
{
	x *= scale;
	y *= scale;
	z *= scale;
}

inline const Vector3 Vector3::operator/( const Vector3& other ) const
{
	return Vector3( x / other.x, y / other.y, z / other.z );
}

inline const Vector3 Vector3::operator/( const Real scale ) const
{
	Real scaleInv = 1.f / scale;
	return ( *this ) * scaleInv;
}

inline void Vector3::operator/=( const Vector3& rhs )
{
	x /= rhs.x;
	y /= rhs.y;
	z /= rhs.z;
}

inline void Vector3::operator/=( const Real scale )
{
	x /= scale;
	y /= scale;
	z /= scale;
}

inline Real Vector3::dot( const Vector3& other ) const
{
	return ( x * other.x + y * other.y + z * other.z );
}

inline Vector3 Vector3::cross( const Vector3& other ) const
{
	return Vector3( y*other.z - other.y*z,
					other.x*z - x*other.z,
					x*other.y - other.x*y );
}

inline Real Vector3::length() const
{
	return sqrt( dot( *this ) );
}

inline Real Vector3::lengthSquared() const
{
	return dot( *this );
}

inline Vector3 Vector3::getRotatedDir( const Real angle ) const
{
	return Vector3(
		cos( angle )*x - sin( angle )*y,
		sin( angle )*x + cos( angle )*y );
}

inline Vector3 Vector3::getNormalized() const
{
	Vector3 res( *this );
	return res / length();
}

inline Vector3 Vector3::getNegated() const
{
	return Vector3( x * -1.f, y * -1.f );
}

inline Vector3 Vector3::getNegated( unsigned int idx ) const
{
	Assert( idx == 0 || idx == 1, "Trying to negate invalid index" );
	Vector3 res( *this );

	if ( idx == 0 )
	{
		res.x *= -1.0f;
	}
	else
	{
		res.y *= -1.0f;
	}

	return res;
}


inline Vector3 Vector3::getTripleCrossProduct( const Vector3& v1, const Vector3& v2 ) const
{
	/// Implements (this x v1) x v2 
	Vector3 ab = this->cross( v1 );
	return ab.cross( v2 );
}

///
/// Don't allow construction and returns
inline void Vector3::normalize()
{
	setNormalized( *this );
}

inline void Vector3::negate()
{
	setNegated( *this );
}

///
/// Don't allow construction and returns
inline void Vector3::setAdd( const Vector3& a, const Vector3& b )
{
	x = a.x + b.x;
	y = a.y + b.y;
	z = a.z + b.z;
}

inline void Vector3::setSub( const Vector3& a, const Vector3& b )
{
	x = a.x - b.x;
	y = a.y - b.y;
	z = a.z - b.z;
}

inline void Vector3::setMul( const Vector3& a, const Vector3& b )
{
	x = a.x * b.x;
	y = a.y * b.y;
	z = a.z * b.z;
}

inline void Vector3::setMul( const Vector3& v, const Real scale )
{
	x = v.x * scale;
	y = v.y * scale;
	z = v.z * scale;
}

inline void Vector3::setAddMul( const Vector3& v0, const Vector3& v1, const Real scale )
{ // *this = v0 + v1 * scale
	x = v0.x + v1.x * scale;
	y = v0.y + v1.y * scale;
	z = v0.z + v1.z * scale;
}

inline void Vector3::setSubMul( const Vector3& v0, const Vector3& v1, const Real scale )
{ // *this = v0 - v1 * scale
	x = v0.x - v1.x * scale;
	y = v0.y - v1.y * scale;
	z = v0.z - v1.z * scale;
}

inline void Vector3::setDiv( const Vector3& a, const Vector3& b )
{
	Assert( fabs( b.x ) > std::numeric_limits<Real>::epsilon(), "Division by 0" );
	Assert( fabs( b.y ) > std::numeric_limits<Real>::epsilon(), "Division by 0" );
	x = a.x / b.x;
	y = a.y / b.y;
	z = a.z / b.z;
}

inline void Vector3::setDiv( const Vector3& v, const Real scale )
{
	Assert( fabs( scale ) > std::numeric_limits<Real>::epsilon(), "Division by 0" );
	x = v.x / scale;
	y = v.y / scale;
	z = v.z / scale;
}

inline void Vector3::setDot( const Vector3& a, const Vector3& b )
{
	x = a.x * b.x + a.y * b.y;
	y = x;
	z = x;
}

inline void Vector3::setCross( const Vector3& a, const Vector3& b )
{
	x = a.y*b.z - b.y*a.z;
	y = b.x*a.z - a.x*b.z;
	z = a.x*b.y - b.x*a.y;
}

inline void Vector3::setRotatedDir( const Vector3& v, const Real angle )
{
	Real xbuf = cos( angle )*v.x - sin( angle )*v.y;
	y = sin( angle )*v.x + cos( angle )*v.y;
	x = xbuf;
}

inline void Vector3::setRotatedDir( const Real angle )
{
	Real xbuf = cos( angle )*x - sin( angle )*y;
	y = sin( angle )*x + cos( angle )*y;
	x = xbuf;
}

inline void Vector3::setNormalized( const Vector3& v )
{
	Assert( !v.isZero(), "Trying to normalize a zero vector" );
	setDiv( v, length() );
}

inline void Vector3::setNegated( const Vector3& v )
{
	x = v.x * -1.f;
	y = v.y * -1.f;
	z = v.z * -1.f;
}

inline void Vector3::setNegated( const Vector3& v, unsigned int idx )
{
	Assert( idx == 0 || idx == 1, "Trying to negate invalid index" );

	if ( idx == 0 )
	{
		x = v.x * -1.0f;
		y = v.y;
	}
	else
	{
		x = v.x;
		y = v.y * -1.0f;
	}
}

inline void Vector3::setInterpolate( const Vector3& a, const Vector3& b, const Real t )
{
	*this = a * ( 1.f - t ) + b * t;
}

inline void Vector3::setZero()
{
	x = 0.f;
	y = 0.f;
	z = 0.f;
}

inline void Vector3::setMax( const Vector3 & v )
{
	x = std::max( x, v.x );
	y = std::max( y, v.y );
	z = std::max( z, v.z );
}

inline void Vector3::setMin( const Vector3 & v )
{
	x = std::min( x, v.x );
	y = std::min( y, v.y );
	z = std::min( z, v.z );
}

///
/// Comparison operator overloads
inline bool Vector3::operator > ( const Vector3& rhs ) const
{
	if ( x > rhs.x && y > rhs.y && z > rhs.z )
	{
		return true;
	}

	return false;
}

inline bool Vector3::operator > ( const Real rhs ) const
{
	if ( x > rhs && y > rhs && z > rhs )
	{
		return true;
	}

	return false;
}

inline bool Vector3::operator < ( const Vector3& rhs ) const
{
	return !( *this > rhs );
}

inline bool Vector3::operator < ( const Real rhs ) const
{
	return !( *this > rhs );
}

inline bool Vector3::operator == ( const Vector3& rhs ) const
{
	return !( *this != rhs );
}

inline bool Vector3::operator == ( const Real rhs ) const
{
	return !( *this != rhs );
}

inline bool Vector3::operator != ( const Vector3& rhs ) const
{
	if ( fabs( x - rhs.x ) < std::numeric_limits<Real>::epsilon() &&
		 fabs( y - rhs.y ) < std::numeric_limits<Real>::epsilon() &&
		 fabs( z - rhs.z ) < std::numeric_limits<Real>::epsilon() )
	{
		return false;
	}

	return true;
}

inline bool Vector3::operator != ( const Real rhs ) const
{
	if ( fabs( x - rhs ) < std::numeric_limits<Real>::epsilon() &&
		 fabs( y - rhs ) < std::numeric_limits<Real>::epsilon() &&
		 fabs( z - rhs ) < std::numeric_limits<Real>::epsilon() )
	{
		return false;
	}

	return true;
}

inline Real Vector3::val() const
{
	return x;
}

inline bool Vector3::isZero() const
{
	return ( fabs( x ) < sqrt( std::numeric_limits<Real>::epsilon() ) &&
			 fabs( y ) < sqrt( std::numeric_limits<Real>::epsilon() ) &&
			 fabs( z ) < sqrt( std::numeric_limits<Real>::epsilon() ) );
}

/// TODO: could potentially move to Vector3Utils
inline bool Vector3::isNan() const
{
	return isnan( x ) || isnan( y ) || isnan( z );
}

/// TODO: could potentially move to Vector3Utils
inline bool Vector3::isInf() const
{
	return isinf( x ) || isinf( y ) || isinf( z );
}