#pragma once

#include <vector>
#include <Base.hpp>

#include <physicsBody.hpp> // For physicsMotionType
#include <physicsShape.hpp> // For physicsShape::NUM_SHAPES
#include <physicsTypes.hpp>

typedef void ( *ColliderFuncPtr )( const physicsBody&, const physicsBody&, std::vector<ContactPoint>& );

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

struct JointCinfo
{
	int bodyIdA;
	int bodyIdB;
	Vector3 pivot;
};

class physicsWorld : public physicsObject
{
public: // todo: make these private to prevent accidental change

	physicsWorld(const physicsWorldCinfo& cinfo);
	~physicsWorld();

	BodyId createBody( const physicsBodyCinfo& cinfo );
	void removeBody(BodyId bodyId);
	int addJoint(const JointCinfo& config);
	void removeJoint(JointId jointId);
    void step();
	
	const physicsBody& getBody(BodyId bodyId) const;
	physicsBody& getBody(BodyId bodyId);
	const std::vector<physicsBody>& getBodies() { return m_bodies; }

    void render();

    // Utility funcs
	void setPosition(BodyId bodyId, const Vector3& point);
	void setMotionType(BodyId bodyId, physicsMotionType type);

	// Setters & getters
	const Real getDeltaTime();

private:

	Vector3 m_gravity;
	Real m_cor;
	std::vector<physicsBody> m_bodies; /// Simulated, free bodies
	class physicsSolver* m_solver;
	ColliderFuncPtr m_dispatchTable[physicsShape::NUM_SHAPES][physicsShape::NUM_SHAPES];
	std::vector<BodyIdPair> m_existingPairs; /// Broadphase existing pairs
	std::vector<BodyIdPair> m_lastFrameNewPairs; /// Broadphase new pairs
	std::vector<CollidedPair> m_existingCollidedPairs;
	std::vector<CollidedPair> m_newCollidedPairs;

	std::vector<BodyId> m_activeBodyIds; /// Simulated body Ids
	std::vector<SolverBody> m_solverBodies;
	BodyId m_firstFreeBodyId;

private:

	void registerColliderFunc(physicsShape::Type typeA, physicsShape::Type typeB, ColliderFuncPtr func);
	ColliderFuncPtr getCollisionFunc(const physicsBody& bodyA, const physicsBody& bodyB);

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
