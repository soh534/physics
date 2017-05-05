#pragma once

#include <Base.hpp>
#include <vector>

typedef unsigned short BodyId;
typedef unsigned short JointId;
const BodyId invalidId = -1;

/// TODO: these shouldn't be included for api users

struct BodyIdPair
{
	BodyId bodyIdA;
	BodyId bodyIdB;

	BodyIdPair( const BodyId a = invalidId, const BodyId b = invalidId );
	BodyIdPair( const BodyIdPair& other );
	void set( const BodyId a, const BodyId b );
	void set( const BodyIdPair& other );
};

bool operator == (const BodyIdPair& pairA, const BodyIdPair& pairB);
bool operator < (const BodyIdPair& pairA, const BodyIdPair& pairB);
bool operator > (const BodyIdPair& pairA, const BodyIdPair& pairB);
bool bodyIdPairLess(const BodyIdPair& pairA, const BodyIdPair& pairB);

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
	bool apply; /// TODO: do this properly, make struct for caches specifically
};

struct CollidedPair : public BodyIdPair
{
	std::vector<struct ContactPoint> contactPoints;

	CollidedPair( const BodyId a, const BodyId b );
	CollidedPair( const BodyIdPair& other );
};

struct ContactPoint
{
private:

	Real m_depth;
	Vector3 m_posA; /// Contact on A local to A
	Vector3 m_posB; /// Contact on B local to B
	Vector3 m_norm; /// Pointing from B to A local to world

public:

	ContactPoint(
		Real depth,
		const Vector3& posA,
		const Vector3& posB,
		Vector3 norm)
		: m_depth(depth), m_posA(posA), m_posB(posB), m_norm(norm) {}

	inline const Real getDepth() const { return m_depth; }
	inline const Vector3& getContactA() const { return m_posA; }
	inline const Vector3& getContactB() const { return m_posB; }
	inline const Vector3& getNormal() const { return m_norm; }
};
