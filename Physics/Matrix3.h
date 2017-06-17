#pragma once

class Vector3;

// Transformation matrix class to multiply to vectors to transform them
class Matrix3
{
	// Structure:
	// m_data[0][0], m_data[0][1], m_data[0][2]
	// m_data[1][0], m_data[1][1], m_data[1][2]
	// m_data[2][0], m_data[2][1], m_data[2][2]
	Real m_data[3][3];

public:

	Matrix3();

	Matrix3( const Vector3& position, const Real rotation );

	inline const Real& operator()( int i, int j ) const;

	inline Real& operator()( int i, int j );

	inline void setIdentity();

	inline void setZero();

	// Sets this matrix to transpose of m
	inline void setTranspose( const Matrix3& m );

	// Overwrites translation of this matrix
	inline void setTranslationUnit( const Vector3& translation );

	inline Vector3 getTranslationUnit() const;

	// Overwrites rotation of this matrix
	inline void setRotationUnit( const Real rotation );

	inline Real getRotationUnit() const;

	// Completely resets this matrix into a translation matrix
	inline void setTranslation( const Vector3& translation );

	// Completely resets this matrix into a rotation matrix
	inline void setRotation( const Real rotation );

	// Completely resets this matrix into a translation + rotation matrix
	inline void setTransform( const Vector3& translation, const Real rotation );

	inline void setReflection( const Vector3& direction );

	// Matrix utility functions
	inline void setMul( const Matrix3& m0, const Matrix3& m1 );

	inline void mul( const Matrix3& m );

	inline void setAdd( const Matrix3& m0, const Matrix3& m1 );

	inline void setSub( const Matrix3& m0, const Matrix3& m1 );

	inline void setInverse22( const Matrix3& m );

	inline void setCrossMatrix( const Vector3& v );

	inline void setDiagonalMatrix( const Vector3& d );

	inline void setDiagonalMatrix( const Real& r0, const Real& r1, const Real& r2 );
};

#include <Matrix3.inl>
