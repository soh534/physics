#pragma once

#include <Base.hpp>

#include <physicsObject.hpp>

#include <vector>
#include <array>

/// TODO: since no more agents make these not a class?

class physicsCollider : public physicsObject
{
public:

	enum Types
	{
		CIRCLE_CIRCLE = 0,
		CIRCLE_BOX,
		BOX_BOX,
		CONVEX_CONVEX,
		NUM_COLLIDERS
	};
	
public:

    physicsCollider();

	static void collide(
		const physicsBody* bodyA,
		const physicsBody* bodyB, 
		std::vector<struct ContactPoint>& contacts);
};

class physicsCircleCollider : public physicsCollider
{
private:
    physicsCircleCollider();

public:
	static void collide( const physicsBody* bodyA,
						 const physicsBody* bodyB,
						 std::vector<ContactPoint>& contacts );
};

class physicsCircleBoxCollider : public physicsCollider
{
private:
    physicsCircleBoxCollider();

public:
	static void collide( const physicsBody* bodyA,
						 const physicsBody* bodyB,
						 std::vector<ContactPoint>& contacts );
};

class physicsBoxCollider : public physicsCollider
{
private:
	physicsBoxCollider();

public:
	static void collide( const physicsBody* bodyA,
						 const physicsBody* bodyB,
						 std::vector<ContactPoint>& contacts );
};

class physicsConvexCollider: public physicsCollider
{
private:
	physicsConvexCollider();

public:
	static void collide( const physicsBody* bodyA,
						 const physicsBody* bodyB,
						 std::vector<ContactPoint>& contacts );

	static void getSimplexVertex( const Vector3& direction,
								  const physicsBody* bodyA,
								  const physicsBody* bodyB,
								  Vector3& vert,
								  Vector3& supportA,
								  Vector3& supportB );

	static void expandingPolytopeAlgorithm( const physicsBody* bodyA,
											const physicsBody* bodyB,
											std::vector< std::array<Vector3, 3> >& simplex,
											struct SimplexEdge& closest );
	
	static void getClosestEdgeToOrigin( const std::vector< std::array<Vector3, 3> >& simplex,
										struct SimplexEdge& edge );
};
