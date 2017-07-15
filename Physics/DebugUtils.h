#pragma once

// Debugging macros
//#define D_BROADPHASE

//#define D_TANGENTIAL_IMPULSE
//#define D_CONTACT_IMPULSE

// Narrowphase
//#define D_GJK_MINKOWSKI
//#define D_GJK_CONTACT_LENGTH
//#define D_GJK_SIMPLEX
//#define D_EPA_SIMPLEX

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

	// Narrowphase
	void drawMinkowskiDifference( const std::shared_ptr<physicsShape>& shapeA,
								  const std::shared_ptr<physicsShape>& shapeB,
								  const Transform& transformA,
								  const Transform& transformB );
	void drawContactNormal( const Vector3& contactA, const Vector3& normal );
	void drawSimplex( const physicsConvexCollider::Simplex& simplex, unsigned int color );
	void drawExpandedSimplex( const physicsConvexCollider::Simplex& simplex );
}
