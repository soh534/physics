#ifndef PHYSICS_WORLD_HPP
#define PHYSICS_WORLD_HPP

#include <vector>
#include <Base.hpp>

#include <physicsBody.hpp> // For physicsMotionType
#include <physicsShape.hpp> // For physicsShape::NUM_SHAPES
#include <physicsTypes.hpp>

typedef class physicsCollider* (*CreateColliderFunc)();

struct physicsWorldCinfo
{
	Vector3 m_gravity;
	Real m_deltaTime;
	Real m_cor;
	int m_numIter;

	physicsWorldCinfo() :
		m_gravity(0.f, -98.f),
		m_deltaTime(.016f),
		m_cor(1.f),
		m_numIter(8) {}
};

struct JointConfig
{
	int bodyIdA;
	int bodyIdB;
	Vector3 armA;
	Vector3 armB;
	Vector3 pivot;
};

class physicsWorld : public physicsObject
{
public: // todo: make these private to prevent accidental change

	physicsWorld(const physicsWorldCinfo& cinfo);
	~physicsWorld();

	BodyId addBody(physicsBody* const body);
	int addJoint(const JointConfig& config);
	void removeBody(BodyId bodyId);
	void removeJoint(JointId jointId);
    void step();
	
	physicsBody const * const getBody(BodyId bodyId) const;
	physicsBody* getBody(BodyId bodyId);
	const std::vector<physicsBody*>& getBodies() { return m_bodies; }

    void render();

    // Utility funcs
	void setPosition(BodyId bodyId, const Vector3& point);
	void setMotionType(BodyId bodyId, physicsMotionType type);

	// Setters & getters
	const Real getDeltaTime();

private:

	Vector3 m_gravity;
	Real m_cor;
	std::vector<physicsBody*> m_bodies;
	class physicsSolver* m_solver;
	CreateColliderFunc m_dispatchTable[physicsShape::NUM_SHAPES][physicsShape::NUM_SHAPES];
	std::vector<BodyIdPair> m_existingPairs; // Broadphase existing pairs
	std::vector<BodyIdPair> m_newPairs; // Broadphase new pairs
	std::vector<CollidedPair> m_existingCollidedPairs;
	std::vector<CollidedPair> m_newCollidedPairs;

private:

	void registerColliderCreateFunc(physicsShape::Type typeA, physicsShape::Type typeB, CreateColliderFunc func);
	physicsCollider* createCollider(physicsBody const * const bodyA, physicsBody const * const bodyB);

	void stepSolve(
		const std::vector<CollidedPair>& existingCollisionsIn,
		const std::vector<CollidedPair>& newCollisionsIn,
		const std::vector<BodyIdPair>& lostCollisionsIn);

	void stepCollide(
		std::vector<CollidedPair>& existingCollisionsOut,
		std::vector<CollidedPair>& newCollisionsOut,
		std::vector<BodyIdPair>& lostCollisionsOut
	);
	void broadPhase(std::vector<BodyIdPair>& broadPhasePassedPairsOut);
	void narrowPhase(const std::vector<BodyIdPair>& bbPassedPairsIn, std::vector<CollidedPair>& collisionsOut);

	bool checkCollidable(BodyId bodyIdA, BodyId bodyIdB);
};

#endif
