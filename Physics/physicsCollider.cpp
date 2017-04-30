#include <Base.hpp>
#include <physicsTypes.hpp>

#include <cassert>
#include <iostream>
#include <physicsCd.hpp>
#include <physicsBody.hpp>
#include <physicsShape.hpp>
#include <physicsObject.hpp>
#include <physicsCollider.hpp>

#include <DebugUtils.hpp>
#include <Renderer.hpp>

const Real g_collisionTolerance = .5f;
const Real g_convexRadius = 1.f;
const int g_gjkMaxIter = 50;
const Real g_tolerance = 0.001f;

/// Base collision agent class functions
physicsCollider::physicsCollider()
{

}

void physicsCollider::collide(
	const physicsBody* bodyA, 
	const physicsBody* bodyB,
	std::vector<ContactPoint>& contacts)
{

}

/// Circle-circle collision agent class functions
physicsCircleCollider::physicsCircleCollider()
{

}

void physicsCircleCollider::collide(
	const physicsBody* bodyA, 
	const physicsBody* bodyB,
	std::vector<ContactPoint>& contacts)
{
	{
		physicsShape* shapeA = bodyA->getShape();
		physicsShape* shapeB = bodyB->getShape();
		Assert(shapeA->getType() == physicsShape::CIRCLE, "non-circle shape sent to circle collider");
		Assert(shapeB->getType() == physicsShape::CIRCLE, "non-circle shape sent to circle collider");
	}

	physicsCircleShape* shapeA = (physicsCircleShape*)bodyA->getShape();
    physicsCircleShape* shapeB = (physicsCircleShape*)bodyB->getShape();

    Real radA = shapeA->getRadius();
    Real radB = shapeB->getRadius();

	Vector3 posA = bodyA->getPosition();
	Vector3 posB = bodyB->getPosition();

	Vector3 ab = posB - posA;

	if (ab.checkZero())
	{
		return;
	}

	Real len = ab.length();
	Real depth = radA + radB - len;

	if (depth > 0.f)
    {
		/// TODO: has problem when a circle is exactly overlapping another
		Vector3 abHat = ab / len;
		Vector3 baHat = abHat.getNegated();

		Vector3 cpA = posA + abHat * radA;
		Vector3 cpB = posB + baHat * radB;
		Vector3 norm = cpB - cpA;
		
		if (norm.checkZero())
		{
			/// TODO: Treated as touching due to numerical error
			return;
		}
		
		norm.normalize();

		ContactPoint contact(
			depth,
			cpA - posA,
			cpB - posB,
			norm
		); /// AB for separation

		contacts.push_back(contact);
    }
}

/// Circle-box collision agent class functions
physicsCircleBoxCollider::physicsCircleBoxCollider()
{

}

/// A: Circle, B: Box
void physicsCircleBoxCollider::collide(
	const physicsBody* bodyA,
	const physicsBody* bodyB,
	std::vector<ContactPoint>& contacts)
{
	{
		physicsShape* shapeA = bodyA->getShape();
		physicsShape* shapeB = bodyB->getShape();
		Assert(shapeA->getType() == physicsShape::CIRCLE, "non-circle shape sent to circle-box collider 1st param");
		Assert(shapeB->getType() == physicsShape::BOX, "non-box shape sent to circle-box collider 2nd param");
	}
}

/// Box-box collision agent class functions
physicsBoxCollider::physicsBoxCollider()
{

}

void physicsBoxCollider::collide(
	const physicsBody* bodyA, 
	const physicsBody* bodyB,
	std::vector<ContactPoint>& contacts)
{
	{
		physicsShape* shapeA = bodyA->getShape();
		physicsShape* shapeB = bodyB->getShape();
		Assert(shapeA->getType() == physicsShape::BOX, "non-box shape sent to box collider");
		Assert(shapeB->getType() == physicsShape::BOX, "non-box shape sent to box collider");
	}
}

/// Convex-convex collision agent class functions
physicsConvexCollider::physicsConvexCollider()
{

}

void physicsConvexCollider::getSimplexVertex(
	const Vector3& direction,
	const physicsBody* bodyA,
	const physicsBody* bodyB,
	Vector3& vert,
	Vector3& supportA,
	Vector3& supportB)
{
	Vector3 dirLocalA = direction.getRotatedDir(-1.f * bodyA->getRotation());
	Vector3 dirLocalB = direction.getNegated().getRotatedDir(-1.f * bodyB->getRotation());

	physicsShape* shapeA = bodyA->getShape();
	physicsShape* shapeB = bodyB->getShape();
	shapeA->getSupportingVertex(dirLocalA, supportA);
	shapeB->getSupportingVertex(dirLocalB, supportB);

	Matrix3 localAtoWorld, localBtoWorld;
	localAtoWorld.setTransform(bodyA->getPosition(), bodyA->getRotation());
	localBtoWorld.setTransform(bodyB->getPosition(), bodyB->getRotation());

	supportA.setTransformedPos(localAtoWorld, supportA);
	supportB.setTransformedPos(localBtoWorld, supportB);
    vert = supportA - supportB;
}

// bool physicsConvexConvexAgent::containsOrigin(std::vector<Vector3f>& simplexVertices, Vector2f& direction) const
// {
//     Vector3f A = simplexVertices.back();
//     Vector3f AO = A.negated();

//     int numSimplexVertices = (int)simplexVertices.size();
//     if (numSimplexVertices == 3)
//     {
//         Vector3f B = simplexVertices[0];
//         Vector3f C = simplexVertices[1];
        
//         renderLine(A, B, Color::GREEN);
//         renderLine(A, C, Color::GREEN);
//         renderLine(B, C, Color::GREEN);

//         Vector2f AB = B.sub(A);
//         Vector2f AC = C.sub(A);

//         Vector2f ABperp = AC.tripleProduct(AB, AB).normalized();
//         Vector2f ACperp = AB.tripleProduct(AC, AC).normalized();

//         if (ABperp.dot(AO) > 0.0f)
//         {
//             simplexVertices.erase(simplexVertices.begin());
//             direction = ABperp;
//         }
//         else if (ACperp.dot(AO) > 0.0f)
//         {
//             simplexVertices.erase(simplexVertices.begin() + 1);
//             direction = ACperp;
//         }
//         else {
//             return true;
//         }
//     }
//     else if (numSimplexVertices == 2)
//     {
//         Vector2f AB = simplexVertices[0].sub(A);
//         direction = (AB.tripleProduct(AO, AB)).normalized();
//     }
//     else
//     {
//         assert(false);
//     }

//     return false;
// }

struct SimplexVertex
{
	Vector3 vert, p0, p1;
};

struct SimplexEdge
{
	int index;
	Real distSq;
	Vector3 normal;
};

void physicsConvexCollider::collide(
	const physicsBody* bodyA,
	const physicsBody* bodyB,
	std::vector<ContactPoint>& contacts)
{
    Vector3 posA = bodyA->getPosition();
    Vector3 posB = bodyB->getPosition();

    Vector3 direction = posB - posA;

	if (direction.checkZero())
	{
		return;
	}

	direction.setNormalized(direction);

	/// [Simplex vertex index][0=simplex, 1=supportA, 2=supportB]
	std::vector< std::array<Vector3, 3> > simplex(3);

    getSimplexVertex(direction, bodyA, bodyB, simplex[0][0], simplex[0][1], simplex[0][2]);
	direction.setNegated(direction);
    getSimplexVertex(direction, bodyA, bodyB, simplex[1][0], simplex[1][1], simplex[1][2]);

    Vector3 origin(0.0f, 0.0f);

	physicsCd::calcClosestPointOnLine(simplex[0][0], simplex[1][0], origin, direction);

	/// TODO: find out appropriate eps
	/// float eps = sqrt(std::numeric_limits<float>::epsilon());
	Real eps = 0.01f;
    
    for (int i = 0; i < g_gjkMaxIter; i++)
    {
		/// TODO: fix
		/// DebugUtils::drawMinkowskiDifference(this);

		/// TODO: Fix bug where direction becomes IND000

		if (direction.checkZero())
		{
			/// Origin is on the simplex
			return;
		}

        direction.setNegated(direction);
        direction.setNormalized(direction);

		/// Get third simplex triangle vertex
        getSimplexVertex(direction, bodyA, bodyB, simplex[2][0], simplex[2][1], simplex[2][2]);

		{ 
			/// Terminate when origin is enclosed by triangle simplex
			Vector3 edge01; edge01.setSub(simplex[1][0], simplex[0][0]);
			Vector3 edge12; edge12.setSub(simplex[2][0], simplex[1][0]);
			Vector3 edge20; edge20.setSub(simplex[0][0], simplex[2][0]);
			Vector3 vo;

			vo.setNegated(simplex[0][0]);
			bool l01 = (edge01(0)*vo(1) - edge01(1)*vo(0)) > 0;
			vo.setNegated(simplex[1][0]);
			bool l12 = (edge12(0)*vo(1) - edge12(1)*vo(0)) > 0;
			vo.setNegated(simplex[2][0]);
			bool l20 = (edge20(0)*vo(1) - edge20(1)*vo(0)) > 0;

			if (l01 == l12 && l12 == l20)
			{
				break;
			}
		}

        Real da = direction.dot(simplex[0][0]);
        Real dc = direction.dot(simplex[2][0]);

        if (dc - da < eps)
        {
			/// Converged on closest feature on simplex
			Vector3 L; L.setSub(simplex[1][0], simplex[0][0]);

			Vector3 pointA, pointB;
			Vector3 normal;

            if (L.checkZero())
            {
				/// Closest simplex feature is a point
				pointA = simplex[0][1];
				pointB = simplex[0][2];
            }
            else
            {
				/// Determine closest point on simplex edge
                Real l = -1.f * simplex[0][0].dot(L) / L.dot(L);

				/// Interpolation to find contact details if closest is on edge of simplex
				pointA.setInterpolate(simplex[0][1], simplex[1][1], l);
				pointB.setInterpolate(simplex[0][2], simplex[1][2], l);
            }

			normal.setSub(pointA, pointB);
			if (normal.checkZero())
			{
				/// Exclude touching situations
				return;
			}
			normal.setNormalized(normal);

			/// TODO: Implement convex radius here
			ContactPoint contact(
				0.f, 
				(pointA - posA).getRotatedDir(-bodyA->getRotation()),
				(pointB - posB).getRotatedDir(-bodyB->getRotation()),
				//pointA - posA,
				//pointB - posB,
				normal
			);

			DebugUtils::drawContactLength(pointA, pointB, normal);

            return;
        }

		/// Drive simplex edge closer to origin
		Vector3 closest02, closest21;
		if (simplex[0][0] == simplex[2][0])
		{
			closest02 = simplex[0][0];
		}
		else
		{
			physicsCd::calcClosestPointOnLine(simplex[0][0], simplex[2][0], origin, closest02);
		}

		if (simplex[2][0] == simplex[1][0])
		{
			closest21 = simplex[1][0];
		}
		else
		{
			physicsCd::calcClosestPointOnLine(simplex[2][0], simplex[1][0], origin, closest21);
		}

		if (closest02.dot(closest02) < closest21.dot(closest21))
		{
			simplex[1][0] = simplex[2][0];
			simplex[1][1] = simplex[2][1];
			simplex[1][2] = simplex[2][2];
			direction = closest02;
		}
		else
		{
			simplex[0][0] = simplex[2][0];
			simplex[0][1] = simplex[2][1];
			simplex[0][2] = simplex[2][2];
			direction = closest21;
		}
    }

	DebugUtils::drawTerminationSimplex(simplex);

	SimplexEdge closest;
	expandingPolytopeAlgorithm(bodyA, bodyB, simplex, closest);

	DebugUtils::drawExpandedSimplex(simplex);

	// Determine closest point on simplex edge
	const size_t szSimplex = simplex.size();
	unsigned int i, j;
	j = closest.index;
	i = j == 0 ? szSimplex - 1 : j - 1;

	Vector3 pointA, pointB;

	// Todo: Find out why two same vertices exist in simplex
	Vector3 L = simplex[j][0] - simplex[i][0];
	if (L.checkZero())
	{
		return;
	}

	Real l = -1.f * simplex[i][0].dot(L) / L.dot(L);

	// Interpolation to find contact details if closest is on edge of simplex
    /*
	drawCross(simplex[i][1], 10.f, RED);
	drawCross(simplex[j][1], 10.f, BLUE);
	drawCross(simplex[i][2], 10.f, RED);
	drawCross(simplex[j][2], 10.f, BLUE);
    */

	pointA.setInterpolate(simplex[i][1], simplex[j][1], l);
	pointB.setInterpolate(simplex[i][2], simplex[j][2], l);

	// Must be directed from A to B because penetration
	Vector3 normal = pointB - pointA;

	if (normal.checkZero())
	{
		return;
		/// Shapes aren't penetrated
	}

	DebugUtils::drawContactLength(pointA, pointB, normal);

	ContactPoint contact(
		normal.length(),
		(pointA - posA).getRotatedDir(-bodyA->getRotation()),
		(pointB - posB).getRotatedDir(-bodyB->getRotation()),
		//pointA - posA,
		//pointB - posB,
		normal.getNormalized()
	);

	contacts.push_back(contact);
}

void physicsConvexCollider::expandingPolytopeAlgorithm(
	const physicsBody* bodyA,
	const physicsBody* bodyB,
	std::vector< std::array<Vector3, 3> >& simplex,
	SimplexEdge& edge)
{	
	const Vector3 origin(0.f, 0.f);

	while (true)
	{
		// TODO: freezes here in some collisions
		getClosestEdgeToOrigin(simplex, edge);

		Vector3 vert, p0, p1;
		getSimplexVertex(edge.normal, bodyA, bodyB, vert, p0, p1);

		Real dist = vert.dot(edge.normal);

		if (dist - edge.distSq < g_tolerance)
		{ 
			// Convergence, closest edge determined
			break;
		}
		else
		{
			// Expand simplex
			std::array<Vector3, 3> newSimplexVertex = { vert, p0, p1 };
			std::vector< std::array<Vector3, 3> >::iterator iter = simplex.begin();
			simplex.insert(iter + edge.index, newSimplexVertex);
		}
	}
}

void physicsConvexCollider::getClosestEdgeToOrigin(
	const std::vector< std::array<Vector3, 3> >&  simplex,
	SimplexEdge& edge )
{
	edge.distSq = std::numeric_limits<Real>::max();
	int szSimplex = (int)simplex.size();

	Vector3 origin;

	for (int i = 0; i < szSimplex; i++)
	{
		int j = (i + 1 == szSimplex) ? 0 : i + 1;

		Vector3 edgeCw = simplex[j][0] - simplex[i][0];

		// Get vector from origin to edge, which is direction we want to expand to
		Vector3 n = edgeCw.cross(edgeCw.cross(simplex[i][0]));
		n.negate();

		// For cases where simplex is touching the origin
		if (!n.checkZero())
		{
			n.normalize();
		}

		Real len = n.dot(simplex[i][0]);

		if (len < edge.distSq)
		{
			edge.distSq = len;
			edge.normal = n;
			edge.index = j;
		}
	}
}