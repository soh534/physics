#pragma once

// Debugging macros
#define D_BROADPHASE

//#define D_TANGENTIAL_IMPULSE
//#define D_CONTACT_IMPULSE

// Narrowphase
//#define D_GJK_MINKOWSKI
//#define D_GJK_CONTACT_LENGTH
//#define D_GJK_TERMINATION_SIMPLEX
//#define D_GJK_EXPANDED_SIMPLEX

// Solver
#define D_SOLVER_IMPULSE

#include <Renderer.h>

#include <sstream>

#include <physicsBody.h>
#include <physicsCollider.h>

class Vector3;

namespace DebugUtils
{
	// Broadphase
	void drawBpAabb( const physicsBody& body );
	void debug_print_line( const Vector3& p0, const Vector3& p1 );
	void debug_draw_triangle( const Vector3& p0, const Vector3& p1, const Vector3& p2 );

	// Narrowphase
	void drawMinkowskiDifference( physicsConvexCollider const * const agent );
	void drawContactLength( const Vector3& pA, const Vector3& pB, const Vector3& n );
	void drawTerminationSimplex( const std::vector< std::array<Vector3, 3> >& simplex );
	void drawExpandedSimplex( const std::vector< std::array<Vector3, 3> >& simplex );

	// Solver
	void drawBodyImpulse( physicsBody const * const body, const Vector3& arm, const Vector3& impulse );
}
