#pragma once

class Vector4;

// Homogeneous transformation matrix which stores 2d translation and z-axis rotation in a 3-by-3
// Capable of multiplying with other transformation matrices to produce combined transforms

class Transform
{
	// Structure:
	// m_data[0][0], m_data[0][1], m_data[0][2]
	// m_data[1][0], m_data[1][1], m_data[1][2]
	// m_data[2][0], m_data[2][1], m_data[2][2]

	Real m_data[3][3];
	Real m_rotation; // in radians
	
public:

	Transform();

	// Sets into T = Lt * Rt ( Rotation applied then translation )
	Transform( const Vector4& translation, const Real rotation );

	inline const Real& operator()( int i, int j ) const;

	inline void setIdentity();

	// Sets translational component of this matrix
	inline void addTranslation( const Vector4& translation );

	// Sets into pure translation matrix
	inline void setTranslation( const Vector4& translation );

	// Gets translational component of this matrix
	// If transform also has rotation and was inverted, naturally inverse(Lt) != -Lt
	inline Vector4 getTranslation() const;

	// Sets rotational component of this matrix
	inline void addRotation( const Real rotation );

	// Sets into pure rotation matrix
	inline void setRotation( const Real rotation );

	// Gets rotational component of this matrix
	// Always inverse(Rt) == -Rt
	inline Real getRotation() const;

	// Sets into matrix with translational and rotational components
	inline void setTransform( const Vector4& translation, const Real rotation );

	// Sets into pure reflection matrix across direction
	inline void setReflection( const Vector4& direction );

	// Matrix utility functions
	inline void setMul( const Transform& t0, const Transform& t1 );

	inline void mul( const Transform& t );

	inline void setInverse( const Transform& t );

	inline void invert();

	inline bool isApproximatelyEqual( const Transform& t,
									  Real epsilon = std::numeric_limits<Real>::epsilon() );

private:

	inline void setZero();
};

#include <Common/Transform.inl>
