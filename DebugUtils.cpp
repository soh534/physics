#include <Base.hpp>

#include <DebugUtils.hpp>

void DebugUtils::drawBpAabb(physicsBody const * const body)
{
#if defined D_BROADPHASE
	const physicsAabb& aabb = body->getAabb();
	drawBox(aabb.m_max, aabb.m_min, RED);
#endif
}

void DebugUtils::debug_print_line(const Point3& p0, const Point3& p1)
{
#if defined (D_GJK_CONTACT_LENGTH)
	drawLine(p0, p1);
	std::stringstream ss;
	ss << (p0 - p1).length() << std::endl;
	Point3 pos;
	pos.setAdd(p0, p1);
	pos.setDiv(pos, 2.f);
	drawText(ss.str(), pos);
#endif
}

void DebugUtils::debug_draw_triangle(const Point3& p0, const Point3& p1, const Point3& p2)
{
#if defined (D_GJK_CONTACT_LENGTH)
	drawLine(p0, p1, RED);
	drawLine(p1, p2, RED);
	drawLine(p2, p0, RED);
#endif
}

void DebugUtils::drawMinkowskiDifference(physicsConvexCollider const * const agent)
{
#if defined D_GJK_MINKOWSKI
	Point3 vertex0, vertex1, dummy0, dummy1;
	Vector3 direction(1.0f, 0.0f);

	agent->getSimplexVertex(direction, vertex0, dummy0, dummy1);

	const int numIntervals = 10;
	const Real degPerInterval = 360.f / numIntervals;

	for (int i = 1; i < numIntervals; i++)
	{
		direction.setRotatedDir(direction, degPerInterval);
		agent->getSimplexVertex(direction, vertex1, dummy0, dummy1);

		drawLine(vertex0, vertex1, GREEN);

		vertex0 = vertex1;
	}
#endif
}

void DebugUtils::drawContactLength(const Point3& pA, const Point3& pB, const Vector3& n)
{
#if defined D_GJK_CONTACT_LENGTH
	drawLine(pA, pB, BLUE);
	std::stringstream ss;
	ss << n.length() << std::endl;
	Point3 pos = (pA + pB) * .5f;
	drawText(ss.str(), pos);
#endif
}

void DebugUtils::drawTerminationSimplex(const std::vector< std::array<Point3, 3> >& simplex)
{
#if defined (D_GJK_TERMINATION_SIMPLEX)	
	debug_draw_triangle(simplex[0][0], simplex[1][0], simplex[2][0]);
#endif
}

void DebugUtils::drawExpandedSimplex(const std::vector< std::array<Point3, 3> >& simplex)
{
#if defined (D_GJK_EXPANDED_SIMPLEX)
	int szSimplex = (int)simplex.size();
	for (int i = 0; i < szSimplex; i++)
	{
		int j = i + 1 == szSimplex ? 0 : i + 1;
		drawLine(simplex[i][0], simplex[j][0], BLUE);
		std::stringstream ss;
		ss << i << std::endl;
		drawText(ss.str(), simplex[i][0]);
	}
#endif
}

void DebugUtils::drawBodyImpulse(physicsBody const * const body, const Vector3& arm, const Vector3& impulse)
{
#if defined (D_SOLVER_IMPULSE)
	drawArrow(body->getPosition() + arm, impulse, RED);
#endif
}