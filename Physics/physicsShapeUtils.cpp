#include <physicsShape.h>
#include <physicsShapeUtils.h>

void physicsShapeUtils::getClosestEdgeToPoint( const physicsBoxShape& box,
											   const Vector3& point,
											   Vector3& base,
											   Vector3& edge )
{
	Vector3 dirNe, dirSe;
	Vector3 halfExtents = box.getHalfExtents();
	dirNe = halfExtents;
	dirSe.setNegated( halfExtents, 1 );

	Real dotTopRight = dirNe.dot( point );
	Real dotBottomRight = dirSe.dot( point );

	if ( dotTopRight > 0.0f && dotBottomRight > 0.0f )
	{ // Right edge
		base = dirSe;
		edge.set( 0.0f, 2.0f * halfExtents( 1 ) );
	}
	else if ( dotTopRight > 0.0f && dotBottomRight < 0.0f )
	{ // Up edge
		base = dirNe;
		edge.set( -2.0f * halfExtents( 0 ), 0.0f );
	}
	else if ( dotTopRight < 0.0f && dotBottomRight > 0.0f )
	{ // Bottom edge
		dirNe.setNegated( dirNe );
		base = dirNe;
		edge.set( 2.0f * halfExtents( 0 ), 0.0f );
	}
	else if ( dotTopRight < 0.0f && dotBottomRight < 0.0f )
	{ // Left edge
		dirSe.setNegated( dirSe );
		base = dirSe;
		edge.set( 0.0f, -2.0f * halfExtents( 1 ) );
	}
}
