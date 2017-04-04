#ifndef PHYSICS_CD_HPP
#define PHYSICS_CD_HPP

#include <Base.hpp>

namespace physicsCd
{
	/// Finds closest point in line 'p1' to 'p2' from 'point'
	void calcClosestPointOnLine(const Point3& p1, const Point3& p2, const Point3& point, Point3& closest);

	/// Finds intersection point 
	void calcLineLineIntersection(const Point3& l0p0, const Point3& l0p1, const Point3& l1p0, const Point3& l1p1, Point3& intersection);

	/// Cast ray against triangle
	void castRay3DTri(const Point3& ra, const Point3& rb, const Point3& ta, const Point3& tb, const Point3& tc, Point3& res);
}

#endif
