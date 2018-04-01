#include <Common/Base.h>

// Transformation matrix class non-inline functions

Transform::Transform()
{
	setIdentity();
}

Transform::Transform( const Vector4& position, const Real rotation )
{
	setIdentity();
	addTranslation( position );
	addRotation( rotation );
}