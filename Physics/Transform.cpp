#include <Base.h>

/// Transformation matrix class non-inline functions

Transform::Transform()
{
	setIdentity();
}

Transform::Transform( const Vector3& position, const Real rotation )
{
	setIdentity();
	addTranslation( position );
	addRotation( rotation );
}