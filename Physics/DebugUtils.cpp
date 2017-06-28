#include <Base.h>

#include <DebugUtils.h>

/// TODO: these debug macros should be checked for in the calling function
/// instead of having these functions being called @ default

void DebugUtils::drawBpAabb( const physicsBody& body )
{
#if defined D_BROADPHASE
	const physicsAabb& aabb = body.getAabb();
	drawBox( aabb.m_max, aabb.m_min, RED );
#endif
}

void DebugUtils::debug_print_line( const Vector3& p0, const Vector3& p1 )
{
#if defined (D_GJK_CONTACT_LENGTH)
	drawLine( p0, p1 );
	Vector3 pos = p0 + p1;
	drawText( std::to_string( ( p0 - p1 ).length() ), pos / 2.f );
#endif
}

void DebugUtils::drawMinkowskiDifference( const physicsConvexCollider* agent )
{
#if defined D_GJK_MINKOWSKI
	Vector3 vertex0, vertex1, dummy0, dummy1;
	Vector3 direction( 1.0f, 0.0f );

	agent->getSimplexVertex( direction, vertex0, dummy0, dummy1 );

	const int numIntervals = 10;
	const Real degPerInterval = 360.f / numIntervals;

	for ( int i = 1; i < numIntervals; i++ )
	{
		direction.setRotatedDir( direction, degPerInterval );
		agent->getSimplexVertex( direction, vertex1, dummy0, dummy1 );

		drawLine( vertex0, vertex1, LIME );

		vertex0 = vertex1;
	}
#endif
}

void DebugUtils::drawContactNormal( const Vector3& contactA, const Vector3& normal )
{
	drawArrow( contactA, normal, OLIVE );
	drawText( std::to_string( normal.length() ), contactA + normal / 2 );
}

void DebugUtils::drawSimplex( const physicsConvexCollider::Simplex& simplex )
{
	drawCross( simplex[0][0], 45.f * g_degToRad, 15.f, RED );
	drawCross( simplex[1][0], 45.f * g_degToRad, 15.f, RED );
	drawCross( simplex[2][0], 45.f * g_degToRad, 15.f, RED );
	drawLine( simplex[0][0], simplex[1][0], RED );
	drawLine( simplex[1][0], simplex[2][0], RED );
	drawLine( simplex[2][0], simplex[0][0], RED );
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

void DebugUtils::drawBodyImpulse( const physicsBody* body, const Vector3& arm, const Vector3& impulse )
{
#if defined (D_SOLVER_IMPULSE)
	drawArrow( body->getPosition() + arm, impulse, RED );
#endif
}