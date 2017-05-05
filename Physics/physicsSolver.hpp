#pragma once

#include <vector>

struct ConstrainedPair;
struct CollidedPair;
class physicsBody;

struct SolverBody
{
	Vector3 v;
	Vector3 w;
	Vector3 pos;
	Real ori;
	Real mInv;
	Real iInv;
};

/// TODO: think of more appropriate places to put this
struct ConstrainedPair : public BodyIdPair
{
	std::vector<Constraint> constraints;

	ConstrainedPair( const BodyId a, const BodyId b );
	ConstrainedPair( const BodyIdPair& other );
};

class physicsSolver
{
public:

	physicsSolver( const Real deltaTime, const int numIterations );
	~physicsSolver();

	BodyId addSolverBody( const physicsBody& body );
	SolverBody& getSolverBody( const BodyId bodyId );

	int addJointConstraint( const ConstrainedPair& joint );
	void removeJointConstraint( JointId jointId );

	void addNewContacts( const std::vector<CollidedPair>& newCollisionsIn );
	void updateContacts( const std::vector<CollidedPair>& remainingCollisionsIn );
	void updateJointConstraints( const std::vector<physicsBody*>& bodies );


	void preSolve(
		const std::vector<CollidedPair>& existingCollisionsIn,
		const std::vector<CollidedPair>& newCollisionsIn,
		const std::vector<BodyIdPair>& lostCollisionsIn,
		const std::vector<physicsBody*>& bodies );
	void solveConstraints( std::vector<physicsBody*>& updatedBodiesOut );
	void solveConstraintPairs( std::vector<ConstrainedPair>& pairsIn, bool contact );

	const Real getDeltaTime() { return m_deltaTime; }
	const int getNumIterations() { return m_numIter; }

private:

	Real m_deltaTime;
	int m_numIter;
	std::vector<ConstrainedPair> m_contactConstraintPairs;
	std::vector<ConstrainedPair> m_jointConstraintPairs;
	std::vector<SolverBody> m_solverBodies;
};

