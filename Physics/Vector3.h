#pragma once

class Matrix3;

class Vector3
{
public: /// For debugging purpose
	Real x, y, z;
public:
	Vector3();
	Vector3( const Real x, const Real y, const Real z = 0.f );
	Vector3( const Vector3& copy );

	///
	/// Setting
	inline void operator=( const Vector3& other );

	inline void set( const Real a, const Real b, const Real c = 0.f );

	inline void setAll( const Real val );

	///
	/// Getting
	inline Real& operator()( int i );

	inline const Real& operator()( int i ) const;

	///
	/// Allow construction and returns, based on other
	inline const Vector3 operator+( const Vector3& other ) const;

	inline void operator+=( const Vector3& rhs );

	inline const Vector3 operator-( const Vector3& other ) const;

	inline void operator-=( const Vector3& rhs );

	inline const Vector3 operator*( const Vector3& other ) const;

	inline const Vector3 operator*( const Real scale ) const;

	inline void operator*=( const Vector3& rhs );

	inline void operator*=( const Real scale );

	inline const Vector3 operator/( const Vector3& other ) const;

	inline const Vector3 operator/( const Real scale ) const;

	inline void operator/=( const Vector3& rhs );

	inline void operator/=( const Real scale );

	inline Real dot( const Vector3& other ) const;

	inline Vector3 cross( const Vector3& other ) const;

	inline Real length() const;

	inline Real lengthSquared() const;

	inline Vector3 getRotatedDir( const Real a ) const;

	inline Vector3 getNormalized() const;

	inline Vector3 getNegated() const;

	inline Vector3 getNegated( unsigned int idx ) const;

	/// Implements (this x v1) x v2
	inline Vector3 getTripleCrossProduct( const Vector3& v1, const Vector3& v2 ) const;

	///
	/// Don't allow construction and returns

	inline void normalize();

	inline void negate();

	///
	/// Don't allow construction and returns, modify this based on other
	inline void setAdd( const Vector3& a, const Vector3& b );

	inline void setSub( const Vector3& a, const Vector3& b );

	inline void setMul( const Vector3& a, const Vector3& b );

	inline void setMul( const Vector3& v, const Real scale );

	inline void setAddMul( const Vector3& v0, const Vector3& v1, const Real scale );

	inline void setSubMul( const Vector3& v0, const Vector3& v1, const Real scale );

	inline void setDiv( const Vector3& a, const Vector3& b );

	inline void setDiv( const Vector3& v, const Real scale );

	inline void setDot( const Vector3& a, const Vector3& b );

	inline void setCross( const Vector3& a, const Vector3& b );

	inline void setRotatedDir( const Vector3& v, const Real angle );

	inline void setNormalized( const Vector3& v );

	inline void setNegated( const Vector3& v );

	inline void setNegated( const Vector3& v, unsigned int idx );

	inline void setInterpolate( const Vector3& a, const Vector3& b, const Real t );

	inline void setZero();

	///
	/// Comparison operator overloads
	inline bool operator > ( const Vector3& rhs ) const;

	inline bool operator > ( const Real rhs ) const;

	inline bool operator < ( const Vector3& rhs ) const;

	inline bool operator < ( const Real rhs ) const;

	inline bool operator == ( const Vector3& rhs ) const;

	inline bool operator == ( const Real rhs ) const;

	inline bool operator != ( const Vector3& rhs ) const;

	inline bool operator != ( const Real rhs ) const;

	inline Real val() const;

	inline bool isZero() const;

	///
	/// Sanity checks
	inline bool isNotNan() const;

	///
	/// Non-inline functions
	void setTransformedPos( const Matrix3& t, const Vector3& v );

	void setTransformedInversePos( const Matrix3& t, const Vector3& v );

	void setClampedLength( const Vector3& v, const Real length );

	///
	/// Debugging ostream function
	friend std::ostream& operator<<( std::ostream& stream, Vector3& v );
};

#include <Vector3.inl>
