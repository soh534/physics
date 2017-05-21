#pragma once

#include <Base.h>

#include <physicsObject.h>

#include <vector>
#include <array>

struct ContactPoint
{
private:

	Real m_depth;
	Vector3 m_posA; /// Contact on A local to A
	Vector3 m_posB; /// Contact on B local to B
	Vector3 m_norm; /// Pointing from B to A local to world

public:

	ContactPoint( Real depth, const Vector3& posA, const Vector3& posB, Vector3 norm )
		: m_depth( depth ), m_posA( posA ), m_posB( posB ), m_norm( norm ) {}

	inline const Real getDepth() const { return m_depth; }
	inline const Vector3& getContactA() const { return m_posA; }
	inline const Vector3& getContactB() const { return m_posB; }
	inline const Vector3& getNormal() const { return m_norm; }
};

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
		const physicsBody& bodyA,
		const physicsBody& bodyB, 
		std::vector<struct ContactPoint>& contacts);
};

class physicsCircleCollider : public physicsCollider
{
private:

    physicsCircleCollider();

public:

	static void collide( const physicsBody& bodyA,
						 const physicsBody& bodyB,
						 std::vector<ContactPoint>& contacts );
};

class physicsCircleBoxCollider : public physicsCollider
{
private:

    physicsCircleBoxCollider();

public:

	static void collide( const physicsBody& bodyA,
						 const physicsBody& bodyB,
						 std::vector<ContactPoint>& contacts );
};

class physicsBoxCollider : public physicsCollider
{
private:

	physicsBoxCollider();

public:

	static void collide( const physicsBody& bodyA,
						 const physicsBody& bodyB,
						 std::vector<ContactPoint>& contacts );
};

class physicsConvexCollider: public physicsCollider
{
private:

	physicsConvexCollider();

public:

	static void collide( const physicsBody& bodyA,
						 const physicsBody& bodyB,
						 std::vector<ContactPoint>& contacts );

	static void getSimplexVertex( const Vector3& direction,
								  const physicsBody& bodyA,
								  const physicsBody& bodyB,
								  Vector3& vert,
								  Vector3& supportA,
								  Vector3& supportB );

	static void expandingPolytopeAlgorithm( const physicsBody& bodyA,
											const physicsBody& bodyB,
											std::vector< std::array<Vector3, 3> >& simplex,
											struct SimplexEdge& closest );
	
	static void getClosestEdgeToOrigin( const std::vector< std::array<Vector3, 3> >& simplex,
										struct SimplexEdge& edge );
};
