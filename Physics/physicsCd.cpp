#include <physicsCd.h>

// Finds closest point on line 'p1'---'p2' to 'point'
void physicsCd::calcClosestPointOnLine(const Vector4& p1, const Vector4& p2, const Vector4& point, Vector4& closest)
{
	Assert(p1 != p2, "Edge is a point");
	Vector4 p1Point; p1Point.setSub(point, p1);
	Vector4 p1P2; p1P2.setSub(p2, p1);
	
	Real lambda = p1Point.dot<2>(p1P2) / p1P2.dot<2>(p1P2);
	
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

void physicsCd::calcLineLineIntersection(const Vector4& l0p0, const Vector4& l0p1, const Vector4& l1p0, const Vector4& l1p1, Vector4& intersection)
{

}

void physicsCd::castRay3DTri(const Vector4& ra, const Vector4& rb, const Vector4& ta, const Vector4& tb, const Vector4& tc, Vector4& res)
{
	Vector4 norm = (tb - ta).cross(tc - ta);
	Vector4 m = rb - ra;
	Real hitFraction = (norm.dot<2>(ta) - norm.dot<2>(ra)) / norm.dot<2>(m);
	res.setAddMul(ra, m, hitFraction);
}
