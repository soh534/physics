#include <memory>
#include <cassert>
#include <iostream>
#include <Base.h>
#include <physicsTypes.h>
#include <physicsCd.h>
#include <physicsBody.h>
#include <physicsShape.h>
#include <physicsObject.h>
#include <physicsCollider.h>

#include <DebugUtils.h>
#include <Renderer.h>

const Real g_collisionTolerance = .5f;
const Real g_convexRadius = 1.f;
const int g_gjkMaxIter = 50;
const Real g_tolerance = 0.01f;

void ContactPointUtils::getContactDifference( const ContactPoint& cpA, const ContactPoint& cpB, Real& res )
{
	const Vector4& cpaa = cpA.getContactA();
	const Vector4& cpab = cpA.getContactB();
	const Vector4& cpba = cpB.getContactA();
	const Vector4& cpbb = cpB.getContactB();
	
	res = ( cpaa - cpba ).lengthSquared<2>() + ( cpab - cpbb ).lengthSquared<2>();
}


// Base collision agent class functions
physicsCollider::physicsCollider()
{

}

void physicsCollider::collide( const std::shared_ptr<physicsShape>& shapeA, 
							   const std::shared_ptr<physicsShape>& shapeB,
							   const Transform & transformA, const Transform & transformB,
							   std::vector<ContactPoint>& contacts )
{

}

// Circle-circle collision agent class functions
physicsCircleCollider::physicsCircleCollider()
{

}

void physicsCircleCollider::collide( const physicsShape* shapeA,
									 const physicsShape* shapeB, 
									 const Transform & transformA, const Transform & transformB, 
									 std::vector<ContactPoint>& contacts )
{
	Assert( shapeA->getType() == physicsShape::CIRCLE, "non-circle shape sent to circle collider" );
	Assert( shapeB->getType() == physicsShape::CIRCLE, "non-circle shape sent to circle collider" );

	const physicsCircleShape* circleA = static_cast< const physicsCircleShape* >( shapeA );
	const physicsCircleShape* circleB = static_cast< const physicsCircleShape* >( shapeB );

	Vector4 posA = transformA.getTranslation();
	Vector4 posB = transformB.getTranslation();
	Transform rotA; rotA.setRotation( transformA.getRotation() );
	Transform rotB; rotB.setRotation( transformB.getRotation() );

	Vector4 ab = posB - posA;

	if ( ab.isZero() )
	{
		return;
	}

	Real len = ab.length<2>(); // TODO: see if we can avoid square rooting
	Real radA = circleA->getRadius();
	Real radB = circleB->getRadius();
	Real depth = radA + radB - len;

	if ( depth > 0.f )
	{
		Vector4 abHat = ab / len;
		Vector4 baHat = abHat.getNegated();

		Vector4 cpA = posA + abHat * radA;
		Vector4 cpB = posB + baHat * radB;
		Vector4 norm = cpA - cpB;

		if ( norm.isZero() )
		{
			// Don't add contact if exactly touching
			return;
		}

		// TODO: See if we can avoid square rooting
		norm.normalize<2>();

		Vector4 cpAinA; cpAinA.setTransformedInversePos( rotA, cpA - posA );
		Vector4 cpBinB; cpBinB.setTransformedInversePos( rotB, cpB - posB );
		ContactPoint contact( depth, cpAinA, cpBinB, norm ); // AB for separation

		contacts.push_back( contact );
	}
}

// Circle-box collision agent class functions
physicsCircleBoxCollider::physicsCircleBoxCollider()
{

}

// A: Circle, B: Box
void physicsCircleBoxCollider::collide( const physicsShape* shapeA, const physicsShape* shapeB, const Transform & transformA, const Transform & transformB, std::vector<ContactPoint>& contacts )
{
	Assert( shapeA->getType() == physicsShape::CIRCLE, "non-circle shape sent to circle-box collider 1st param" );
	Assert( shapeB->getType() == physicsShape::BOX, "non-box shape sent to circle-box collider 2nd param" );

	// TODO: implement this
}

// Box-box collision agent class functions
physicsBoxCollider::physicsBoxCollider()
{

}

void physicsBoxCollider::collide( const physicsShape* shapeA, const physicsShape* shapeB, const Transform & transformA, const Transform & transformB, std::vector<ContactPoint>& contacts )
{
	Assert( shapeA->getType() == physicsShape::BOX, "non-box shape sent to box collider" );
	Assert( shapeB->getType() == physicsShape::BOX, "non-box shape sent to box collider" );

	// TODO: implement this
}

// Convex-convex collision agent class functions
physicsConvexCollider::physicsConvexCollider()
{

}

void physicsConvexCollider::getSimplexVertex( const Vector4& direction,
											  const physicsShape* shapeA,
											  const physicsShape* shapeB,
											  const Transform& transformA,
											  const Transform& transformB,
											  SimplexVertex& simplexVertex )
{
	Transform rotationA, rotationB;
	rotationA.setRotation( transformA.getRotation() );
	rotationB.setRotation( transformB.getRotation() );

	Vector4 dirLocalA, dirLocalB;
	dirLocalA.setTransformedInversePos( rotationA, direction );
	dirLocalB.setTransformedInversePos( rotationB, direction.getNegated() );

	Vector4 supportA, supportB;
	shapeA->getSupportingVertex( dirLocalA, supportA );
	shapeB->getSupportingVertex( dirLocalB, supportB );

	Assert( supportA.isOk(), "supportA ain't ok" );
	Assert( supportB.isOk(), "supportB ain't ok" );

	simplexVertex[1].setTransformedPos( transformA, supportA );
	simplexVertex[2].setTransformedPos( transformB, supportB );
	simplexVertex[0] = simplexVertex[1] - simplexVertex[2];
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

void physicsConvexCollider::collide(
	const physicsShape* shapeA,
	const physicsShape* shapeB,
	const Transform& transformA,
	const Transform& transformB,
	std::vector<ContactPoint>& contacts )
{
    Vector4 posA = transformA.getTranslation();
    Vector4 posB = transformB.getTranslation();

    Vector4 direction = posB - posA;

	if ( direction.isZero() )
	{
		return;
	}

	//direction.setNormalized( direction ); // TODO: investigate whether normalization really necessary
	
	// [Simplex vertex index][0=simplex, 1=supportA, 2=supportB]
	Simplex simplex( 3 );

//	drawArrow( transformA.getTranslation(), direction, RED );
	//drawArrow( transformB.getTranslation(), direction.getNegated(), BLUE );

	getSimplexVertex( direction, shapeA, shapeB, transformA, transformB, simplex[0] );
	direction.negate();
	getSimplexVertex( direction, shapeA, shapeB, transformA, transformB, simplex[1] );
//	drawCross( simplex[0][1], 45.f * g_degToRad, 30.f, RED );
	//drawCross( simplex[0][2], 45.f * g_degToRad, 30.f, BLUE );

	const Vector4 origin( 0.f, 0.f );
	
	physicsCd::calcClosestPointOnLine( simplex[0][0], simplex[1][0], origin, direction );

	// TODO: find out appropriate eps
	// float eps = sqrt(std::numeric_limits<float>::epsilon());
	Real eps = 0.1f;
    
	for ( int previousIdx = 0; previousIdx < g_gjkMaxIter; previousIdx++ )
    {
		// TODO: Fix bug where direction becomes IND000

		if ( direction.isZero() )
		{
			// Origin is on the simplex
			return;
		}

		direction.negate();
		//direction.setNormalized( direction );

		// Get third simplex triangle vertex
		getSimplexVertex( direction, shapeA, shapeB, transformA, transformB, simplex[2] );

#if defined D_GJK_SIMPLEX
		//DebugUtils::drawSimplex( simplex );
#endif

		{ 
			// Terminate when origin is enclosed by triangle simplex
			Vector4 edge01; edge01.setSub( simplex[1][0], simplex[0][0] );
			Vector4 edge12; edge12.setSub( simplex[2][0], simplex[1][0] );
			Vector4 edge20; edge20.setSub( simplex[0][0], simplex[2][0] );
			Vector4 vo;

			vo.setNegate( simplex[0][0] );
			bool l01 = ( edge01( 0 )*vo( 1 ) - edge01( 1 )*vo( 0 ) ) > 0;
			vo.setNegate( simplex[1][0] );
			bool l12 = ( edge12( 0 )*vo( 1 ) - edge12( 1 )*vo( 0 ) ) > 0;
			vo.setNegate( simplex[2][0] );
			bool l20 = ( edge20( 0 )*vo( 1 ) - edge20( 1 )*vo( 0 ) ) > 0;

			if ( l01 == l12 && l12 == l20 )
			{
				break;
			}
		}

		Real da = direction.dot<2>( simplex[0][0] );
		Real dc = direction.dot<2>( simplex[2][0] );

		if ( dc - da < eps )
        {
			// Converged on closest feature on simplex
			Vector4 L = simplex[1][0] - simplex[0][0];

			Vector4 pointA, pointB;

			if ( L.isZero() )
            {
				// Closest simplex feature is a point
				pointA = simplex[0][1];
				pointB = simplex[0][2];
            }
            else
            {
				// Determine closest point on simplex edge
				Real l = -1.f * simplex[0][0].dot<2>( L ) / L.dot<2>( L );

				// Interpolation to find contact details if closest is on edge of simplex
				pointA.setInterpolate( simplex[0][1], simplex[1][1], l );
				pointB.setInterpolate( simplex[0][2], simplex[1][2], l );
            }

			if ( direction.isZero() )
			{
				// Exclude touching situations
				break;
			}

#if defined D_GJK_CONTACT_LENGTH
			//DebugUtils::drawContactNormal( pointA, direction );
#endif

            return;
        }

		// Drive simplex edge closer to origin
		Vector4 closest02, closest21;
		if ( simplex[0][0] == simplex[2][0] )
		{
			closest02 = simplex[0][0];
		}
		else
		{
			physicsCd::calcClosestPointOnLine( simplex[0][0], simplex[2][0], origin, closest02 );
		}

		if ( simplex[2][0] == simplex[1][0] )
		{
			closest21 = simplex[1][0];
		}
		else
		{
			physicsCd::calcClosestPointOnLine( simplex[2][0], simplex[1][0], origin, closest21 );
		}

		if ( closest02.dot<2>( closest02 ) < closest21.dot<2>( closest21 ) )
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
#if 0
		drawCross( simplex[0][1], 45.f * g_degToRad, 50.f, BLUE );
		drawCross( simplex[1][1], 45.f * g_degToRad, 50.f, RED );
		drawCross( simplex[0][2], 45.f * g_degToRad, 50.f, BLUE );
		drawCross( simplex[1][2], 45.f * g_degToRad, 50.f, RED );
#endif
		if ( previousIdx > 45 )
		{
			previousIdx = previousIdx;
		}
    }

#if defined D_GJK_MINKOWSKI
	DebugUtils::drawMinkowskiDifference( shapeA, shapeB, transformA, transformB );
#endif

	SimplexEdge closestEdge;
	expandingPolytopeAlgorithm( shapeA, shapeB, transformA, transformB, simplex, closestEdge );

	// Determine closest point on simplex edge
	const size_t szSimplex = simplex.size();
	int closestIdx = closestEdge.index;
	auto previousIdx = closestIdx == 0 ? szSimplex - 1 : closestIdx - 1;
	
	// Todo: Find out why two same vertices exist in simplex
	Vector4 L = simplex[closestIdx][0] - simplex[previousIdx][0];
	//drawArrow( simplex[previousIdx][0], L, PURPLE );
	//drawCross( simplex[previousIdx][0], 30.f * g_degToRad, 50.f, RED );
	//drawCross( simplex[closestIdx][0], 30.f * g_degToRad, 50.f, BLUE );
	
	if ( L.isZero() )
	{
		return;
	}
	
	Real l = -1.f * simplex[previousIdx][0].dot<2>( L ) / L.dot<2>( L );
	
	Vector4 pointA, pointB;
	pointA.setInterpolate( simplex[previousIdx][1], simplex[closestIdx][1], l );
	pointB.setInterpolate( simplex[previousIdx][2], simplex[closestIdx][2], l );
	//drawCross( pointA, 30.f * g_degToRad, 50.f, RED );
	// Must be directed from A to B because penetration
	Vector4 normal = closestEdge.normal;
	normal *= closestEdge.distSq;
	
	if ( normal.isZero() )
	{
		return;
		// Shapes aren't penetrated
	}
	
#if defined D_GJK_CONTACT_LENGTH
	//DebugUtils::drawContactNormal( pointA, normal );
#endif
	
	Transform rotationA, rotationB;
	rotationA.setRotation( transformA.getRotation() );
	rotationB.setRotation( transformB.getRotation() );
	
	Vector4 cpInA; cpInA.setTransformedInversePos( rotationA, pointA - posA );
	Vector4 cpInB; cpInB.setTransformedInversePos( rotationB, pointB - posB );
	
	ContactPoint contact( normal.length<2>(), cpInA, cpInB, normal );
	
	contacts.push_back( contact );
	
	// Detect planar contacts
	Transform t;
	const float pt = 15.f * g_degToRad;
	t.setRotation( pt );

	Vector4 d1, d2;
	d1.setTransformedPos( t, closestEdge.normal );
	d2.setTransformedInversePos( t, closestEdge.normal );
	
	SimplexVertex newSimplexVertex1, newSimplexVertex2;
	getSimplexVertex( d1, shapeA, shapeB, transformA, transformB, newSimplexVertex1 );
	getSimplexVertex( d2, shapeA, shapeB, transformA, transformB, newSimplexVertex2 );

	//drawCross( newSimplexVertex1[1], 30.f * g_degToRad, 50.f, BLUE );
	//drawCross( newSimplexVertex1[2], 45.f * g_degToRad, 50.f, BLUE );
	//drawArrow( newSimplexVertex1[0] - d2 * 50.f, d2 * 50.f, BLUE );
	//drawCross( newSimplexVertex2[1], 60.f * g_degToRad, 50.f, RED );
	//drawCross( newSimplexVertex2[2], 75.f * g_degToRad, 50.f, RED );
	//drawArrow( newSimplexVertex2[0] - d2 * 50.f, d2 * 50.f, RED );
	// Determine closest point on simplex edge
	const size_t szSimplexx = simplex.size();
	auto ii = closestEdge.index == 0 ? szSimplexx - 1 : closestEdge.index - 1;
	//drawCross( simplex[ii][0], 45.f * g_degToRad, 50.f, GREEN );
	//drawCross( simplex[ii][1], 45.f * g_degToRad, 50.f, GREEN );
	//drawCross( simplex[ii][2], 45.f * g_degToRad, 50.f, GREEN );
	{
		// Determine closest point on simplex edge
		const size_t szSimplex = simplex.size();
		auto i = closestEdge.index == 0 ? szSimplex - 1 : closestEdge.index - 1;

		// Todo: Find out why two same vertices exist in simplex
		Vector4 L = newSimplexVertex1[0] - simplex[i][0];
		//drawArrow( simplex[i][0], L, BLUE );
		if ( L.isZero() )
		{
			return;
		}

		Real l = -1.f * simplex[i][0].dot<2>( L ) / L.dot<2>( L );

		Vector4 pointA, pointB;
		pointA.setInterpolate( simplex[i][1], newSimplexVertex1[1], l );
		pointB.setInterpolate( simplex[i][2], newSimplexVertex1[2], l );
		//drawCross(pointA, 30.f * g_degToRad, 50.f, BLUE);
		//drawCross(pointB, 60.f * g_degToRad, 50.f, BLUE);
	}

	{
		// Determine closest point on simplex edge
		const size_t szSimplex = simplex.size();
		int i = closestEdge.index == 0 ? szSimplex - 1 : closestEdge.index - 1;

		// Todo: Find out why two same vertices exist in simplex
		Vector4 L = newSimplexVertex2[0] - simplex[i][0];
		//drawArrow( simplex[i][0], L, RED );
		if ( L.isZero() )
		{
			return;
		}

		Real l = -1.f * simplex[i][0].dot<2>( L ) / L.dot<2>( L );

		Vector4 pointA, pointB;
		pointA.setInterpolate( simplex[i][1], newSimplexVertex2[1], l );
		pointB.setInterpolate( simplex[i][2], newSimplexVertex2[2], l );
		//drawCross(pointA, 30.f * g_degToRad, 50.f, RED);
		//drawCross(pointB, 60.f * g_degToRad, 50.f, RED);
	}
}

void physicsConvexCollider::expandingPolytopeAlgorithm(
	const physicsShape* shapeA,
	const physicsShape* shapeB,
	const Transform& transformA,
	const Transform& transformB,
	Simplex& simplex,
	SimplexEdge& closestEdge)
{	
	//int idx = 0;

	//DebugUtils::drawSimplex( simplex, RED );

	while ( true )
	{
		// TODO: investigate not being able to get out of loop in some collisions
		getClosestEdgeToOrigin( simplex, closestEdge );

		SimplexVertex newSimplexVertex;
		getSimplexVertex( closestEdge.normal, shapeA, shapeB, transformA, transformB, newSimplexVertex );

		Real dist = newSimplexVertex[0].dot<2>( closestEdge.normal );
		
		if ( dist - closestEdge.distSq < g_tolerance )
		{ 
			// Convergence, closest edge determined
			break;
		}
		else
		{
			// Expand simplex
			Simplex::iterator iter = simplex.begin();
			simplex.insert( iter + closestEdge.index, newSimplexVertex );
			//drawText( std::to_string( idx ), newSimplexVertex[0], 1.f );
			//idx++;
		}
	}

#if defined D_EPA_SIMPLEX
	DebugUtils::drawSimplex( simplex, BLUE );
#endif
}

void physicsConvexCollider::getClosestEdgeToOrigin( const Simplex& simplex, SimplexEdge& edge )
{
	edge.distSq = std::numeric_limits<Real>::max();
	int szSimplex = (int)simplex.size();

	for ( int i = 0; i < szSimplex; i++ )
	{
		int j = ( i + 1 ) % szSimplex;

		Vector4 edgeCw = simplex[j][0] - simplex[i][0];

		// Get vector from origin to edge, which is direction we want to expand to
		Vector4 n( edgeCw( 1 ), -1.f * edgeCw( 0 ) );
		n.normalize<2>();

		Real len = n.dot<2>( simplex[i][0] );

		if ( len < edge.distSq )
		{
			edge.distSq = len;
			edge.normal = n;
			edge.index = j;
		}
	}
}