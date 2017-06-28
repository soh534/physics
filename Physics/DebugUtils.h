#pragma once

// Debugging macros
//#define D_BROADPHASE

//#define D_TANGENTIAL_IMPULSE
//#define D_CONTACT_IMPULSE

// Narrowphase
//#define D_GJK_MINKOWSKI
#define D_GJK_CONTACT_LENGTH
#define D_GJK_SIMPLEX
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

	// Narrowphase
	void drawMinkowskiDifference( const physicsConvexCollider* agent );
	void drawContactNormal( const Vector3& contactA, const Vector3& normal );
	void drawSimplex( const physicsConvexCollider::Simplex& simplex );
	void drawExpandedSimplex( const physicsConvexCollider::Simplex& simplex );

	// Solver
	void drawBodyImpulse( const physicsBody* body, const Vector3& arm, const Vector3& impulse );
}
