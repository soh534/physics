#ifndef PHYSICS_AGENT_HPP
#define PHYSICS_AGENT_HPP

#include <Base.hpp>

#include <physicsObject.hpp>

#include <vector>
#include <array>

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

	virtual void collide(
		physicsBody const * const bodyA,
		physicsBody const * const bodyB, 
		std::vector<struct ContactPoint>& contacts) = 0;
};

class physicsCircleCollider : public physicsCollider
{
private:
    physicsCircleCollider();

public:

	static physicsCollider* create();

    virtual void collide(
		physicsBody const * const bodyA, 
		physicsBody const * const bodyB,
		std::vector<ContactPoint>& contacts) override;
};

class physicsCircleBoxCollider : public physicsCollider
{
private:
    physicsCircleBoxCollider();

public:
    static physicsCollider* create();

    virtual void collide(
		physicsBody const * const bodyA,
		physicsBody const * const bodyB,
		std::vector<ContactPoint>& contacts
	) override;
};

class physicsBoxCollider : public physicsCollider
{
private:
	  physicsBoxCollider();

public:
	static physicsCollider* create();

    virtual void collide(
		physicsBody const * const bodyA, 
		physicsBody const * const bodyB,
		std::vector<ContactPoint>& contacts
	) override;
};

class physicsConvexCollider: public physicsCollider
{
private:
	physicsConvexCollider();

public:
	static physicsCollider* create();

    void getSimplexVertex(
		const Vector3& direction,
		physicsBody const * const bodyA,
		physicsBody const * const bodyB,
		Point3& vert,
		Point3& supportA,
		Point3& supportB) const;

	virtual void collide(
		physicsBody const * const bodyA, 
		physicsBody const * const bodyB,
		std::vector<ContactPoint>& contacts) override;

	void expandingPolytopeAlgorithm(
		physicsBody const * const bodyA,
		physicsBody const * const bodyB,
		std::vector< std::array<Point3, 3> >& simplex, 
		struct SimplexEdge& closest);

	void getClosestEdgeToOrigin(const std::vector< std::array<Point3, 3> >& simplex, struct SimplexEdge& edge);
};

#endif
