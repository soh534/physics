#pragma once

#include <memory>

#include <physicsObject.h>
#include <physicsTypes.h>
#include <physicsAabb.h>
#include <physicsShape.h>

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

	std::string m_name;
	std::shared_ptr<physicsShape> m_shape;
	physicsMotionType m_motionType;
	Vector3 m_pos;
    Real m_ori;
	Vector3 m_linearVelocity;
    Real m_angularSpeed; /// Radians
    Real m_mass;
	Real m_inertia;
	Vector3 m_com;
	Real m_friction;
	bool m_collidable;
};

struct FreeBody
{
	int m_nextFreeBodyIdx;
};

/// Physical body which allows accessing read-only parameters
/// Changes should be only done internally by the world
class physicsBody : public physicsObject
{
public:

    physicsBody(const physicsBodyCinfo& bodyCinfo);

    ~physicsBody();

	void render() const; /// TODO: move this out of API

	/// Public usage - user-defined name
	inline const std::string& getName() const;
	inline void setName( const std::string& name );

	/// Public usage - bodyId
	inline unsigned int getBodyId() const;

	/// Public usage - shape
	inline std::shared_ptr<physicsShape> getShape() const;

	/// Public usage - motion type
	inline physicsMotionType getMotionType() const;
	inline bool isStatic() const;

	/// Public usage - positions, rotations, linear velocity, angular speed
	inline const Vector3& getPosition() const;
	inline const Real getRotation() const;
	inline const Vector3& getLinearVelocity() const;
	inline const Real getAngularSpeed() const;

	/// Public usage - mass, inertia, COM
	inline const Real getMass() const;
	inline const Real getInertia() const;
	inline const Vector3& getCenterOfMass() const;

	/// Public usage - friction
	inline const Real getFriction() const;

	/// Public usage - queries
	bool containsPoint( const Vector3& point ) const;

private:

	std::string m_name;
	BodyId m_bodyId;
	std::shared_ptr<physicsShape> m_shape;
	physicsMotionType m_motionType;
	Vector3 m_pos;
	Real m_ori;
	Vector3 m_linearVelocity;
	Real m_angularSpeed; /// in radians
	Real m_mass;
	Real m_inertia;
	Vector3 m_com;
	Real m_friction;

public: /// TODO: stuff below should be invisible to users, move it

	/// Internal usage - bodyId
	inline void setBodyId( unsigned int bodyId );

	/// Internal usage - shape
	physicsShape::Type getShapeType() const;

	/// Internal usage - aabb
	inline physicsAabb getAabb() const;
	void updateAabb();

	/// Internal usage - motion type
	inline void setMotionType( physicsMotionType type );

	/// Internal usage - position, rotation, linear velocity, angular speed
	inline Vector3& getPosition();
	inline void setPosition( const Vector3& pos );
	inline Real& getRotation();
	inline void setRotation( const Real rotation );
	inline Vector3& getLinearVelocity();
	inline void setLinearVelocity( const Vector3& linearVel );
	inline Real& getAngularSpeed();
	inline void setAngularSpeed( const Real angularVel );

	void getPointVelocity( const Vector3& arm, Vector3& vel ) const; /// arm is local
	void setDampedVelocity( const Real& damping );

	/// Internal usage - mass, inertia, COM
	inline void setMass(const Real mass);
	inline void setInertia( const Real inertia );
	inline const Real getInvMass() const;
	inline const Real getInvInertia() const;
	inline void setCenterOfMass(const Vector3& com);

	void getInvInertiaWorld( Transform& invInertiaWorld ) const;
	void calcEffectiveMassMatrix( const Vector3& ptWorld, Transform& effMassMatrix ) const;
	void applyPointImpulse( const Vector3& impulse, const Vector3& arm /* arm is in body local*/ );

	/// Internal usage - friction
	inline void setFriction( const Real friction );

	/// Internal usage - indices
	inline void setActiveListIdx( unsigned int idx );
	inline unsigned int getActiveListIdx() const;

	/// Internal usage - collision filter
	inline unsigned int getCollisionFilter() const;

private:

	physicsAabb m_aabb;
	Real m_invMass;
	Real m_invInertia;
	unsigned int m_activeListIdx; /// Index of this body in physicsWorld::m_activeBodyIds
	unsigned int m_collisionFilter;
};

#include <physicsBody.inl>
