#pragma once

#include <vector>
#include <Base.hpp>

#include <physicsBody.hpp> /// For physicsMotionType
#include <physicsShape.hpp> /// For physicsShape::NUM_SHAPES
#include <physicsSolver.hpp>

struct ContactPoint;

typedef void( *ColliderFuncPtr )( const physicsBody&, const physicsBody&, std::vector<ContactPoint>& );

struct physicsWorldCinfo
{
	Vector3 m_gravity;
	Real m_deltaTime;
	Real m_cor;
	int m_numIter;

	physicsWorldCinfo() :
		m_gravity( 0.f, -98.f ),
		m_deltaTime( .016f ),
		m_cor( 1.f ),
		m_numIter( 8 ) {}
};

struct JointCinfo
{
	int bodyIdA;
	int bodyIdB;
	Vector3 pivot;
};

struct CachedPair : public BodyIdPair
{
	CachedPair( const BodyId a, const BodyId b ) : BodyIdPair( a, b ) {}
	CachedPair( const BodyIdPair& other ) : BodyIdPair( other ) {}
};

class physicsWorld : public physicsObject
{
public:

	physicsWorld( const physicsWorldCinfo& cinfo );

	~physicsWorld();

	BodyId createBody( const physicsBodyCinfo& cinfo );

	void removeBody( BodyId bodyId );

	int addJoint( const JointCinfo& config );

	void removeJoint( JointId jointId );

	void step();
	
	int getNumActiveBodies() const { return m_activeBodyIds.size(); };

	void getActiveBodies( std::vector<physicsBody>& activeBodies ) const;

	void render();

	/// Utility funcs
	void setPosition( BodyId bodyId, const Vector3& point );

	void setMotionType( BodyId bodyId, physicsMotionType type );

	const Real getDeltaTime();

protected:

	Vector3 m_gravity;
	Real m_cor;
	std::vector<physicsBody> m_bodies; /// Simulated, free bodies
	SolverInfo m_solverInfo;
	class physicsSolver* m_solver;
	ColliderFuncPtr m_dispatchTable[physicsShape::NUM_SHAPES][physicsShape::NUM_SHAPES];
	std::vector<BodyIdPair> m_newPairs; /// New broadphase pairs
	std::vector<ConstrainedPair> m_cachedContactPairs;
	std::vector<ConstrainedPair> m_jointSolvePairs;
	std::vector<ConstrainedPair> m_contactSolvePairs;
	std::vector<BodyId> m_activeBodyIds; /// Simulated body Ids
	std::vector<SolverBody> m_solverBodies;
	BodyId m_firstFreeBodyId;
};
