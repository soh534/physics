#include <Base.h>

/// Transformation matrix class non-inline functions

Matrix3::Matrix3()
{
	setIdentity();
}

Matrix3::Matrix3( const Vector3& position, const Real rotation )
{
	setIdentity();
	setTranslationUnit( position );
	setRotationUnit( rotation );
}