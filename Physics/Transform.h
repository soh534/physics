#pragma once

class Vector3;

/// Homogeneous transformation matrix which stores 2d translation and z-axis rotation in a 3-by-3
/// Capable of multiplying with other transformation matrices to produce mixed transforms

class Transform
{
	/// Structure:
	/// m_data[0][0], m_data[0][1], m_data[0][2]
	/// m_data[1][0], m_data[1][1], m_data[1][2]
	/// m_data[2][0], m_data[2][1], m_data[2][2]

	Real m_data[3][3];

public:

	Transform();

	Transform( const Vector3& position, const Real rotation );

	inline const Real& operator()( int i, int j ) const;

	inline Real& operator()( int i, int j );

	inline void setIdentity();

	inline void setZero();

	/// Sets this matrix to transpose of m
	inline void setTranspose( const Transform& m );

	/// Sets just the translational component of this matrix
	inline void addTranslation( const Vector3& translation );

	/// Sets into pure translation matrix
	inline void setTranslation( const Vector3& translation );

	/// Gets translational component of this matrix
	inline Vector3 getTranslation() const;

	/// Sets just the rotational component of this matrix
	inline void addRotation( const Real rotation );

	/// Sets into pure rotation matrix
	inline void setRotation( const Real rotation );

	/// Gets rotational component of this matrix
	inline Real getRotation() const;

	/// Sets into matrix with translational and rotational components
	inline void setTransform( const Vector3& translation, const Real rotation );

	/// Sets into pure reflection matrix across direction
	inline void setReflection( const Vector3& direction );

	/// Matrix utility functions
	inline void setMul( const Transform& m0, const Transform& m1 );

	inline void mul( const Transform& m );

	inline void setAdd( const Transform& m0, const Transform& m1 );

	inline void setSub( const Transform& m0, const Transform& m1 );

	inline void setInverse( const Transform& m );

};

#include <Transform.inl>
