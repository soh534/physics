#pragma once

#include <nmmintrin.h>

class Transform;

class Vector4
{
public:

	Vector4();
	Vector4( const Real x, const Real y, const Real z = 0.f );
	Vector4( const Vector4& copy );

	// Setters
	inline void operator=( const Vector4& other );

	inline void set( const Real x, const Real y, const Real z = 0.f, const Real w = 0.f );
	inline void setAll( const Real val );
	inline void setZero();

	inline void setAdd( const Vector4& a, const Vector4& b );
	inline void setSub( const Vector4& a, const Vector4& b );

	inline void setMul( const Vector4& a, const Vector4& b );
	inline void setMul( const Vector4& v, const Real scale );

	// *this = a + b * s
	inline void setAddMul( const Vector4& v0, const Vector4& v1, const Real scale );
	// *this = a - b * s
	inline void setSubMul( const Vector4& v0, const Vector4& v1, const Real scale );

	inline void setDiv( const Vector4& a, const Vector4& b );
	inline void setDiv( const Vector4& a, const Real scale );

	template <int N>
	inline void setDot( const Vector4& a, const Vector4& b );
	inline void setDot3( const Vector4& a, const Vector4& b );
	inline void setDot4( const Vector4& a, const Vector4& b );

	inline void setCross( const Vector4& a, const Vector4& b );

	inline void setRotatedDir( const Vector4& v, const Real angle );
	inline void setRotatedDir( const Real angle );

	template <int N>
	inline void setNormalized( const Vector4& a );
	inline void setNormalized3( const Vector4& a );
	inline void setNormalized4( const Vector4& a );

	inline void setNegate( const Vector4& v );
	inline void setNegate( const Vector4& v, unsigned int idx );

	inline void setMax( const Vector4& v );
	inline void setMin( const Vector4& v );

	inline void setInterpolate( const Vector4& a, const Vector4& b, const Real t );

	// Get single elements
	inline Real& operator()( int i );
	inline const Real& operator()( int i ) const;

	// Arithmetic operators
	inline const Vector4 operator+( const Vector4& other ) const;
	inline void operator+=( const Vector4& rhs );

	inline const Vector4 operator-( const Vector4& other ) const;
	inline void operator-=( const Vector4& rhs );

	inline const Vector4 operator*( const Vector4& other ) const;
	inline const Vector4 operator*( const Real s ) const;
	inline void operator*=( const Vector4& rhs );
	inline void operator*=( const Real rhs );

	inline const Vector4 operator/( const Vector4& other ) const;
	inline const Vector4 operator/( const Real s ) const;
	inline void operator/=( const Vector4& rhs );
	inline void operator/=( const Real rhs );

	// Dot product
	template <int N>
	inline Real dot( const Vector4& other ) const;
	inline Real dot3( const Vector4& other ) const;
	inline Real dot4( const Vector4& other ) const;

	// Cross product
	inline Vector4 cross( const Vector4& other ) const;

	template <int N>
	inline Real length() const;
	inline Real length3() const;
	inline Real length4() const;

	template <int N>
	inline Real lengthSquared() const;
	inline Real lengthSquared3() const;
	inline Real lengthSquared4() const;

	inline Vector4 getRotatedDir( const Real a ) const;

	template <int N>
	inline Vector4 getNormalized() const;
	inline Vector4 getNormalized3() const;
	inline Vector4 getNormalized4() const;

	inline Vector4 getNegated() const;

	inline Vector4 getNegated( unsigned int idx ) const;

	// Implements (this x v1) x v2
	inline Vector4 getTripleCrossProduct( const Vector4& v1, const Vector4& v2 ) const;

	//
	// Don't allow construction and returns

	template <int N>
	inline void normalize();
	inline void normalize3();
	inline void normalize4();

	inline void negate();


	//
	// Comparison operator overloads
	inline bool operator > ( const Vector4& rhs ) const;
	inline bool operator > ( const Real rhs ) const;

	inline bool operator < ( const Vector4& rhs ) const;
	inline bool operator < ( const Real rhs ) const;

	inline bool operator == ( const Vector4& rhs ) const;
	inline bool operator == ( const Real rhs ) const;

	inline bool operator != ( const Vector4& rhs ) const;
	inline bool operator != ( const Real rhs ) const;

	inline bool isZero() const;

	//
	// Sanity checks
	inline bool isNan() const;

	inline bool isInf() const;

	inline bool isOk() const;

	//
	// Non-inline functions
	void setTransformedPos( const Transform& t, const Vector4& v );

	void setTransformedInversePos( const Transform& t, const Vector4& v );

	void setClampedLength( const Vector4& v, const Real length );

	//
	// Debugging ostream function
	friend std::ostream& operator<<( std::ostream& stream, Vector4& v );

	__m128 m_quad;
};

#include <Common/Vector4.inl>
