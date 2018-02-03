#pragma once

#include <Base.h>

namespace physicsCd
{
	// Finds closest point in line 'p1' to 'p2' from 'point'
	void calcClosestPointOnLine(const Vector4& p1, const Vector4& p2, const Vector4& point, Vector4& closest);

	// Finds intersection point 
	void calcLineLineIntersection(const Vector4& l0p0, const Vector4& l0p1, const Vector4& l1p0, const Vector4& l1p1, Vector4& intersection);

	// Cast ray against triangle
	void castRay3DTri(const Vector4& ra, const Vector4& rb, const Vector4& ta, const Vector4& tb, const Vector4& tc, Vector4& res);
}
