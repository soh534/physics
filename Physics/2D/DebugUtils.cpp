#include <Base.h>

#include <DebugUtils.h>
#include <physicsCollider.h>

void DebugUtils::drawMinkowskiDifference( const physicsShape* shapeA,
										  const physicsShape* shapeB,
										  const Transform& transformA,
										  const Transform& transformB )
{
	Vector4 direction( 1.0f, 0.0f );

	physicsConvexCollider::SimplexVertex vert0, vert1;
	physicsConvexCollider::getSimplexVertex( direction, shapeA, shapeB, transformA, transformB, vert0 );

	const int numIntervals = 10;
	const Real degPerInterval = 360.f / numIntervals;

	for ( int i = 1; i <= numIntervals; i++ )
	{
		direction.setRotatedDir( direction, degPerInterval * g_degToRad );
		physicsConvexCollider::getSimplexVertex( direction, shapeA, shapeB, transformA, transformB, vert1 );
		drawLine( vert0[0], vert1[0], GREEN );
		vert0 = vert1;
	}
}

void DebugUtils::drawContactNormal( const Vector4& contactA, const Vector4& normal )
{
	drawArrow( contactA, normal, OLIVE );
	drawText( std::to_string( normal.length<2>() ), contactA + normal / 2 );
}

void DebugUtils::drawSimplex( const physicsConvexCollider::Simplex& simplex, unsigned int color )
{
	int szSimplex = ( int )simplex.size();

	for ( int i = 0; i < szSimplex; i++ )
	{
		drawCross( simplex[i][0], 45.f * g_degToRad, 40.f, color );

		if ( i == szSimplex - 1 )
		{
			drawLine( simplex[szSimplex - 1][0], simplex[0][0], color );
		}
		else
		{
			drawLine( simplex[i][0], simplex[i + 1][0], color );
		}
	}
}

void DebugUtils::drawExpandedSimplex( const physicsConvexCollider::Simplex& simplex )
{
	int szSimplex = ( int )simplex.size();
	for ( int i = 0; i < szSimplex; i++ )
	{
		int j = i + 1 == szSimplex ? 0 : i + 1;
		drawLine( simplex[i][0], simplex[j][0], BLUE );
		std::stringstream ss;
		ss << i << std::endl;
		drawText( ss.str(), simplex[i][0] );
	}
}
