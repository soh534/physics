#ifndef DEBUG_UTILS_HPP
#define DEBUG_UTILS_HPP

// Debugging macros
//#define D_BROADPHASE

//#define D_TANGENTIAL_IMPULSE
//#define D_CONTACT_IMPULSE

// Narrowphase
//#define D_GJK_MINKOWSKI
//#define D_GJK_CONTACT_LENGTH
//#define D_GJK_TERMINATION_SIMPLEX
//#define D_GJK_EXPANDED_SIMPLEX

// Solver
#define D_SOLVER_IMPULSE

#include <Renderer.hpp>

#include <sstream>

#include <physicsBody.hpp>
#include <physicsCollider.hpp>

class Vector3;
typedef Vector3 Point3;

namespace DebugUtils
{
	// Broadphase
	void drawBpAabb(physicsBody const * const body);
	void debug_print_line(const Point3& p0, const Point3& p1);
	void debug_draw_triangle(const Point3& p0, const Point3& p1, const Point3& p2);

	// Narrowphase
	void drawMinkowskiDifference(physicsConvexCollider const * const agent);
	void drawContactLength(const Point3& pA, const Point3& pB, const Vector3& n);
	void drawTerminationSimplex(const std::vector< std::array<Point3, 3> >& simplex);
	void drawExpandedSimplex(const std::vector< std::array<Point3, 3> >& simplex);

	// Solver
	void drawBodyImpulse(physicsBody const * const body, const Vector3& arm, const Vector3& impulse);
}

#endif