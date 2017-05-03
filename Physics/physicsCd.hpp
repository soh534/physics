#pragma once

#include <Base.hpp>

namespace physicsCd
{
	/// Finds closest point in line 'p1' to 'p2' from 'point'
	void calcClosestPointOnLine(const Vector3& p1, const Vector3& p2, const Vector3& point, Vector3& closest);

	/// Finds intersection point 
	void calcLineLineIntersection(const Vector3& l0p0, const Vector3& l0p1, const Vector3& l1p0, const Vector3& l1p1, Vector3& intersection);

	/// Cast ray against triangle
	void castRay3DTri(const Vector3& ra, const Vector3& rb, const Vector3& ta, const Vector3& tb, const Vector3& tc, Vector3& res);
}
