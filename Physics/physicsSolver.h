#pragma once

#include <vector>
#include <physicsInternalTypes.h>

struct Jacobian
{
	Vector3 vA, wA, vB, wB; /// World
};

struct Constraint
{
	Vector3 rA, rB; /// Local
	Real accumImp;
	Real error;
	Jacobian jac;
};

struct ConstrainedPair : public BodyIdPair
{
	std::vector<Constraint> constraints;

	ConstrainedPair( const BodyId a = invalidId, const BodyId b = invalidId ) : BodyIdPair( a, b ) {}
	ConstrainedPair( const BodyIdPair& other ) : BodyIdPair( other ) {}
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
	Vector3 pos;
	Real ori;
	Real mInv;
	Real iInv;

	void setFromBody( const physicsBody& body );
};

class physicsSolver
{
public:

	void solveConstraints( const SolverInfo& info,
						   std::vector<ConstrainedPair>& constraints,
						   std::vector<SolverBody>& solverBodies,
						   std::vector<physicsBody>& physicsBodies );
};
