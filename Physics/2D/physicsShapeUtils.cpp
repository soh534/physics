#include <physicsShape.h>
#include <physicsShapeUtils.h>

void physicsShapeUtils::getClosestEdgeToPoint( const physicsBoxShape& box,
											   const Vector4& point,
											   Vector4& base,
											   Vector4& edge )
{
	Vector4 dirNe, dirSe;
	Vector4 halfExtents = box.getHalfExtents();
	dirNe = halfExtents;
	dirSe.setNegate( halfExtents, 1 );

	Real dotTopRight = dirNe.dot<2>( point );
	Real dotBottomRight = dirSe.dot<2>( point );

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
		dirNe.setNegate( dirNe );
		base = dirNe;
		edge.set( 2.0f * halfExtents( 0 ), 0.0f );
	}
	else if ( dotTopRight < 0.0f && dotBottomRight < 0.0f )
	{ // Left edge
		dirSe.setNegate( dirSe );
		base = dirSe;
		edge.set( 0.0f, -2.0f * halfExtents( 1 ) );
	}
}
