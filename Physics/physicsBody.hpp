#pragma once

#include <memory>

#include <physicsObject.hpp>
#include <physicsTypes.hpp>
#include <physicsAabb.hpp>
#include <physicsShape.hpp>

enum class physicsMotionType
{
	STATIC = 0,
	DYNAMIC,
	KEYFRAME
};

class physicsBodyCinfo
{
public:

    physicsBodyCinfo();
	~physicsBodyCinfo();

public:

	std::shared_ptr<physicsShape> m_shape;
	physicsMotionType m_type;
	Vector3 m_pos;
    Real m_ori;
	Vector3 m_com;
	Vector3 m_linearVelocity;
    Real m_angularSpeed; /// In radians
    Real m_mass;
	Real m_invMass;
	Real m_inertia;
	Real m_invInertia;
	Real m_friction;
	bool m_collidable;
	std::string m_name;
};

struct FreeBody
{
	int m_nextFreeBodyIdx;
};

class physicsBody : public physicsObject
{
  public:
    physicsBody(const physicsBodyCinfo& bodyCinfo);

    ~physicsBody();

    void render() const;

	bool containsPoint(const Vector3& point) const;

    void updateAabb();

	void getPointVelocity(const Vector3& arm /* arm is in body local */, Vector3& vel) const;

	void dampVelocity(const Real& damping);

	void applyPointImpulse(const Vector3& impulse, const Vector3& arm /* arm is in body local*/);

	void getInvInertiaWorld(Matrix3& invInertiaWorld) const;

	void calcEffectiveMassMatrixAt(const Vector3& ptWorld, Matrix3& effMassMatrix) const;

	inline std::shared_ptr<physicsShape> getShape() const;

	inline physicsMotionType getMotionType() const;

	inline void setMotionType(physicsMotionType type);

	inline bool isStatic() const;

	inline const Vector3& getPosition() const;

	inline Vector3& getPosition();

	inline void setPosition(const Vector3& pos);

	inline const Real getRotation() const;

	inline Real& getRotation();

	inline void setRotation(const Real rotation);

	inline const Vector3& getLinearVelocity() const;

	inline Vector3& getLinearVelocity();

	inline void setLinearVelocity(const Vector3& linearVel);

	inline const Vector3& getCenterOfMass() const;

	inline void setCenterOfMass(const Vector3& com);

	inline const Real getAngularSpeed() const;

	inline Real& getAngularSpeed();

	inline void setAngularSpeed(const Real angularVel);

	inline const Real getMass() const;

	inline void setMass(const Real mass);

	inline const Real getInvMass() const;

	inline const Real getInertia() const;

	inline void setInertia(const Real inertia);

	inline const Real getInvInertia() const;

	inline void setFriction(const Real friction);

	inline const Real getFriction() const;

	inline const std::string& getName() const;

	inline void setName(const std::string& name);

	inline physicsAabb getAabb() const;

	inline void setBodyId(unsigned int bodyId);

	inline unsigned int getBodyId() const;

	inline unsigned int getBodyFilter() const;

	physicsShape::Type getShapeType() const;

public:

	std::shared_ptr<physicsShape> m_shape;
	physicsMotionType m_type;
	Vector3 m_pos;
	Real m_ori;
	Vector3 m_com;
	Vector3 m_linearVelocity;
	Real m_angularSpeed; /// Angular speed in radians
	Real m_mass;
	Real m_invMass;
	Real m_inertia;
	Real m_invInertia;
	Real m_friction;
	physicsAabb m_aabb;
	std::string m_name;

	BodyId m_bodyId;
	unsigned int m_bodyFilter;
	unsigned int m_activeListIdx; /// Index of this body in physicsWorld::m_activeBodyIds
};

#include <physicsBody.inl>
