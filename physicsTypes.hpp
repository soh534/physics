#pragma once

#include <Base.hpp>
#include <vector>

typedef unsigned int BodyId;
typedef unsigned int JointId;
const BodyId invalidId = -1;

struct BodyIdPair
{
	BodyId bIdA;
	BodyId bIdB;
};

bool operator == (const BodyIdPair& pairA, const BodyIdPair& pairB);
bool operator < (const BodyIdPair& pairA, const BodyIdPair& pairB);
bool operator > (const BodyIdPair& pairA, const BodyIdPair& pairB);
bool bodyIdPairLess(const BodyIdPair& pairA, const BodyIdPair& pairB);

struct Jacobian
{
	Vector3 vA, wA, vB, wB; // World
};

struct Constraint
{
	Vector3 rA, rB; // Local
	Real accumImp;
	Real error;
	Jacobian jac;
};

struct CollidedPair : public BodyIdPair
{
	std::vector<struct ContactPoint> cp;
};

struct ContactPoint
{
private:

	Real m_depth;
	Point3 m_posA; // Contact on A local to A
	Point3 m_posB; // Contact on B local to B
	Vector3 m_norm; // Pointing from B to A local to world

public:

	ContactPoint(
		Real depth,
		const Point3& posA,
		const Point3& posB,
		Vector3 norm)
		: m_depth(depth), m_posA(posA), m_posB(posB), m_norm(norm) {}

	inline const Real getDepth() const { return m_depth; }
	inline const Point3& getContactA() const { return m_posA; }
	inline const Point3& getContactB() const { return m_posB; }
	inline const Vector3& getNormal() const { return m_norm; }
};
