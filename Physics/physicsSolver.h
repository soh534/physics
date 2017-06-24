#pragma once

#include <vector>
#include <physicsInternalTypes.h>

struct Jacobian
{
	Vector3 vA, wA, vB, wB;
};

struct Constraint
{
	Vector3 rA, rB; /// Constrained points viewed from local
	Real error;
	Jacobian jac;
};

struct ConstrainedPair : public BodyIdPair
{
	std::vector<Constraint> constraints;
	Real accumImp;

	ConstrainedPair( const BodyId a = invalidId, const BodyId b = invalidId ) : 
		BodyIdPair( a, b ), accumImp(0.f) 
	{

	}

	ConstrainedPair( const BodyIdPair& other ) : 
		BodyIdPair( other ), accumImp(0.f)
	{

	}
};

struct SolverInfo
{
	Real m_deltaTime;
	int m_numIter;
};

struct SolverBody
{
	Vector3 v;
	Vector3 w;
	Vector3 pos; /// TODO: remove this
	Real ori; /// TODO: remove this
	Real mInv;
	Real iInv;

	void setFromBody( const physicsBody& body );
};

class physicsSolver
{
public:

	void solveConstraints( const SolverInfo& info,
						   bool isContact,
						   std::vector<ConstrainedPair>& constrainedPairs,
						   std::vector<SolverBody>& solverBodies,
						   std::vector<physicsBody>& physicsBodies );
};
