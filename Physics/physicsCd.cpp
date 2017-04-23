#include <physicsCd.hpp>

/// Finds closest point in line 'p1' to 'p2' from 'point'
void physicsCd::calcClosestPointOnLine(const Vector3& p1, const Vector3& p2, const Vector3& point, Vector3& closest)
{
	Assert(p1 != p2, "Edge is a point");
	Vector3 p1Point; p1Point.setSub(point, p1);
	Vector3 p1P2; p1P2.setSub(p2, p1);
	
	Real lambda = p1Point.dot(p1P2) / p1P2.dot(p1P2);
	
	if (lambda > 1.0f)
	{
		closest = p2;
	}
	else if (lambda < 0.0f)
	{
		closest = p1;
	}
	else
	{
		closest.setMul(p1P2, lambda);
		closest.setAdd(closest, p1);
	}
}

void physicsCd::calcLineLineIntersection(const Vector3& l0p0, const Vector3& l0p1, const Vector3& l1p0, const Vector3& l1p1, Vector3& intersection)
{

}

void physicsCd::castRay3DTri(const Vector3& ra, const Vector3& rb, const Vector3& ta, const Vector3& tb, const Vector3& tc, Vector3& res)
{
	Vector3 norm = (tb - ta).cross(tc - ta);
	Vector3 m = rb - ra;
	Real hitFraction = (norm.dot(ta) - norm.dot(ra)) / norm.dot(m);
	res.setAddMul(ra, m, hitFraction);
}
