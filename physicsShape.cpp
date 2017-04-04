#include <physicsShape.hpp>
#include <physicsAabb.hpp>

#include <vector>
#include <climits>
#include <bitset> // For flags during convex vertices ordering
#include <algorithm>

#define _USE_MATH_DEFINES
#include <math.h>

#include <Renderer.hpp>

const Real g_density = .01f;

///#define D_PRINT_VERTICES
#if defined (D_PRINT_VERTICES)
#include <sstream>
#endif

//
// Base shape class functions
//
physicsShape::physicsShape()
	: m_convexRadius(0.1f)
{

}

physicsShape::~physicsShape()
{

}

Real physicsShape::calculateMass() const
{
	Assert(false, "Shouldn't have been called");
	return -1.f;
}

Real physicsShape::calculateInertia() const
{
	Assert(false, "Shouldn't have been called");
	return -1.f;
}

void physicsShape::render(const Point3& pos, const Real rot) const
{
    Assert(false, "Base shape class constructor shouldn't have been called");
}

bool physicsShape::containsPoint(const Point3& point) const
{
    Assert(false, "Base shape class containsPoint function shouldn't have been called");
    return false;
}

void physicsShape::getSupportingVertex(const Vector3& direction, Point3& point) const
{
    Assert(false, "Base shape class getSupportingVertex function shouldn't have been called");
}

physicsAabb physicsShape::getAabb(const Real rot) const
{
    Assert(false, "Base shape class getAabb function shouldn't have been called");
    return physicsAabb();
}

//
// Circle shape class functions
//
physicsCircleShape::physicsCircleShape(const Real radius)
    : m_radius(radius)
{

}

physicsCircleShape::~physicsCircleShape()
{

}

Real physicsCircleShape::calculateMass() const
{
	Real area = (Real)M_PI * m_radius * m_radius;
	return (g_density * area);
}

Real physicsCircleShape::calculateInertia() const
{
#if 1
	/// TODO: For now use aabb shape
	Real a = m_radius;
	Real b = m_radius;

	return ((4.f / 3.f) * g_density * (a*a*a*b + a*b*b*b));
#else
	return calculateMass();
#endif
}

void physicsCircleShape::render(const Point3& pos, const Real rot) const
{
	/// Circles don't need to have rotation taken into account
    drawCircle(pos, m_radius);
}

bool physicsCircleShape::containsPoint(const Point3& point) const
{
    Real distSq = point.lengthSquared();

	if (distSq < m_radius*m_radius)
    {
        return true;
    }

    return false;
}

void physicsCircleShape::getSupportingVertex(const Vector3& direction, Point3& point) const
{
	point.setMul(direction, m_radius);
}

physicsAabb physicsCircleShape::getAabb(const Real rot) const
{
	Vector3 halfExtent; halfExtent.setAll(m_radius);
	Vector3 halfExtentNeg; halfExtentNeg.setNegated(halfExtent);
    return physicsAabb(halfExtent, halfExtentNeg);
}

/// Box shape class functions
physicsBoxShape::physicsBoxShape(const Vector3& halfExtents)
    : m_halfExtents(halfExtents)
{

}

physicsBoxShape::~physicsBoxShape()
{

}

Real physicsBoxShape::calculateMass() const
{
	return (4.f * m_halfExtents(0) * m_halfExtents(1) * g_density);
}

Real physicsBoxShape::calculateInertia() const
{
	/// From gray book: I = 4/3 p (aaab + abbb)
#if 1
	Real a = m_halfExtents(0);
	Real b = m_halfExtents(1);
	return ((4.f / 3.f) * g_density * (a*a*a*b + a*b*b*b));
#else
	return calculateMass();
#endif
}

void physicsBoxShape::render(const Point3& pos, const Real rot) const
{
	// Replace with multiplication by transformation matrix
	Vector3 posNe, posNw, posSe, posSw;
	posNe.setRotatedDir(m_halfExtents, rot);
	posNe.setAdd(posNe, pos);

	posNw.setNegated(m_halfExtents, 0);
	posNw.setRotatedDir(posNw, rot);
	posNw.setAdd(posNw, pos);

	posSe.setNegated(m_halfExtents, 1);
	posSe.setRotatedDir(posSe, rot);
	posSe.setAdd(posSe, pos);

	posSw.setNegated(m_halfExtents);
	posSw.setRotatedDir(posSw, rot);
	posSw.setAdd(posSw, pos);

    drawLine(posNe, posNw);
	drawLine(posNw, posSw);
	drawLine(posSw, posSe);
	drawLine(posSe, posNe);
}

bool physicsBoxShape::containsPoint(const Point3& point) const
{
    if (-1.0f * m_halfExtents(0) <= point(0) && point(0) <= m_halfExtents(0) &&
        -1.0f * m_halfExtents(0) <= point(1) && point(1) <= m_halfExtents(1))
    {
        return true;
    }

    return false;
}

void physicsBoxShape::getSupportingVertex(const Vector3& direction, Point3& point) const
{
	Vector3 dirNw(-m_halfExtents(0), m_halfExtents(1));
	Vector3 dirSw(-m_halfExtents(0), -m_halfExtents(1));
	Vector3 dirSe(m_halfExtents(0), -m_halfExtents(1));
	Vector3 dirNe(m_halfExtents(0), m_halfExtents(1));

    Real currMax = std::numeric_limits<Real>::lowest();
    Real potentialMaxDot;

    potentialMaxDot = direction.dot(dirNw);
    if (potentialMaxDot > currMax)
    {
		currMax = potentialMaxDot;
        point = dirNw;
    }

    potentialMaxDot = direction.dot(dirSw);
    if (potentialMaxDot > currMax)
    {
		currMax = potentialMaxDot;
        point = dirSw;
    }

    potentialMaxDot = direction.dot(dirSe);
    if (potentialMaxDot > currMax)
    {
		currMax = potentialMaxDot;
        point = dirSe;
    }

    potentialMaxDot = direction.dot(dirNe);
    if (potentialMaxDot > currMax)
    {
		currMax = potentialMaxDot;
        point = dirNe;
    }
}

physicsAabb physicsBoxShape::getAabb(const Real rot) const
{
	/// ERROR: this shouldn't have to convert to radians
	Real radians = rot * (Real)M_PI / 180.0f;
    Real w = 2.0f * m_halfExtents(0) * cos(radians) + 2.0f * m_halfExtents(1) * sin(radians);
    Real h = 2.0f * m_halfExtents(0) * sin(radians) + 2.0f * m_halfExtents(1) * cos(radians);

    return physicsAabb(
		Vector3(w / 2.0f, h / 2.0f),
		Vector3(-w / 2.0f, -h / 2.0f));
}

void physicsBoxShape::getEdgeFacingPoint(const Point3& point, Point3& base, Vector3& edge)
{

	Vector3 dirNe, dirSe;
	dirNe = m_halfExtents;
	dirSe.setNegated(m_halfExtents, 1);

	Real dotTopRight = dirNe.dot(point);
	Real dotBottomRight = dirSe.dot(point);

    if (dotTopRight > 0.0f && dotBottomRight > 0.0f)
    { // Right edge
		base = dirSe;
		edge.set(0.0f, 2.0f * m_halfExtents(1));
    }
    else if (dotTopRight > 0.0f && dotBottomRight < 0.0f)
    { // Up edge
        base = dirNe;
        edge.set(-2.0f * m_halfExtents(0), 0.0f);
    }
    else if (dotTopRight < 0.0f && dotBottomRight > 0.0f)
    { // Bottom edge
		dirNe.setNegated(dirNe);
        base = dirNe;
        edge.set(2.0f * m_halfExtents(0), 0.0f);
    }
    else if (dotTopRight < 0.0f && dotBottomRight < 0.0f)
    { // Left edge
		dirSe.setNegated(dirSe);
        base = dirSe;
        edge.set(0.0f, -2.0f * m_halfExtents(1));
    }
}

//
// Convex shape class functions
//
physicsConvexShape::physicsConvexShape(const std::vector<Point3>& vertices, const Real radius)
{ /// Get unsorted list of vertices, establish connections to treat as convex

	/// TODO: Bug where code will fail if two same vertices exist in vertices array
	int numVertices = (int)vertices.size();

	/// TODO: APPLY CONVEX RADIUS
	m_vertices.assign(vertices.begin(), vertices.end());

	/// Determine connectivity
	unsigned int xMinIdx = 0;

	for (int i = 1; i < numVertices; i++)
	{
		if (m_vertices[xMinIdx](0) > m_vertices[i](0))
		{
			xMinIdx = i;
		}
	}

	m_connectivity.push_back(xMinIdx);

	int nodeCurrent = xMinIdx;
	int nodeNext = -1;

	Vector3 edgeCurrent;
	edgeCurrent.set(0.0f, 1.0f);

	std::vector<bool> flags;
	flags.resize(numVertices, false);

	for (int i = 0; i < numVertices; i++)
	{
		Real cosCurrent = -1.0f;

		for (int j = 0; j < numVertices; j++)
		{
			if (j == nodeCurrent || flags[j])
			{
				continue;
			}

			Vector3 edgePotential;
			edgePotential.setSub(m_vertices[j], m_vertices[nodeCurrent]);

			Real cosPotential = edgeCurrent.dot(edgePotential) / (edgeCurrent.length() * edgePotential.length());

			if (cosPotential > cosCurrent)
			{
				nodeNext = j;
				cosCurrent = cosPotential;
			}
		}

		flags[nodeNext] = true;
		m_connectivity.push_back(nodeNext);
		edgeCurrent.setSub(m_vertices[nodeNext], m_vertices[nodeCurrent]);
		nodeCurrent = nodeNext;
	}
}

physicsConvexShape::~physicsConvexShape()
{

}

Real physicsConvexShape::calculateMass() const
{
	Real area = 0.f;

	const Point3& v0 = m_vertices[m_connectivity[0]];

	for (int i = 1; i < (int)m_vertices.size(); i++)
	{
		area += v0.cross(m_vertices[m_connectivity[1]])(2);
	}

	return g_density * -1.f * area;
}

Real physicsConvexShape::calculateInertia() const
{
#if 1
	/// TODO: For now use aabb shape
	physicsAabb aabb = getAabb(0.f);

	Real a = aabb.m_max(0);
	Real b = aabb.m_max(1);

	return calculateMass() * (4*a*a + 4*b*b) / 12.f;
#else
	return calculateMass();
#endif
}

void physicsConvexShape::render(const Point3& pos, const Real rot) const
{
    int numConnectivity = (int)m_connectivity.size();

	for (int i = 1; i < numConnectivity; i++)
    {
		Vector3 dirV0; dirV0.setRotatedDir(m_vertices[m_connectivity[i-1]], rot);
		Point3 v0; v0.setAdd(pos, dirV0);

		Vector3 dirV1; dirV1.setRotatedDir(m_vertices[m_connectivity[i]], rot);
		Point3 v1; v1.setAdd(pos, dirV1);

		drawLine(v0, v1);

#if defined (D_PRINT_VERTICES)
		std::stringstream ss;
		ss << v0;
		drawText(ss.str(), v0);

		ss.str("");

		ss << v1;
		drawText(ss.str(), v1);
#endif
    }
}

bool physicsConvexShape::containsPoint(const Point3& point) const
{
    Vector3 edge, normal, dirMpToPoint;

	int numConnectivity = (int)m_connectivity.size();

	for (int i = 0; i < numConnectivity - 1; i++)
    {
		edge.setSub(m_vertices[m_connectivity[i + 1]], m_vertices[m_connectivity[i]]);
        normal.set(edge(1), -edge(0));
		
		Point3 midpoint; midpoint.setAddMul(m_vertices[m_connectivity[i]], edge, 0.5f);
		dirMpToPoint.setSub(point, midpoint);

		if (normal.dot(dirMpToPoint) < 0.0f)
        {
            return false;
        }
    }

	return true;
}

void physicsConvexShape::getSupportingVertex(const Vector3& direction, Point3& point) const
{
    Real dotMax = std::numeric_limits<Real>::lowest();
    Real potentialMaxDot;

    int numVertices = (int)m_vertices.size();

    for(int i=0; i<numVertices; i++)
    {
        potentialMaxDot = direction.dot(m_vertices[i]);

		if (potentialMaxDot > dotMax)
        {
            dotMax = potentialMaxDot;
            point = m_vertices[i];
        }
    }
}

physicsAabb physicsConvexShape::getAabb(const Real rot) const
{
    Real xmin, xmax, ymin, ymax;

	xmax = ymax = std::numeric_limits<Real>::lowest();
    xmin = ymin = std::numeric_limits<Real>::max();

    int numVertices = (int)m_vertices.size();
	for (int i = 0; i < numVertices; i++)
    {
		Vector3 vertW = m_vertices[i].getRotatedDir(rot);
		xmin = std::min(vertW(0), xmin);
		xmax = std::max(vertW(0), xmax);
		ymin = std::min(vertW(1), ymin);
		ymax = std::max(vertW(1), ymax);
    }

	return physicsAabb(Vector3(xmax, ymax), Vector3(xmin, ymin));
}
