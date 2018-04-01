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
	Vector4 m_pos;
    Real m_ori;
	Vector4 m_linearVelocity;
    Real m_angularSpeed; // Radians
    Real m_mass;
	Real m_inertia;
	Vector4 m_com;
	Real m_friction;
	bool m_collidable;
};

struct FreeBody
{
	int m_nextFreeBodyIdx;
};

// Physical body which allows accessing read-only parameters
// Changes should be only done internally by the world
class physicsBody : public physicsObject
{
public:

    physicsBody(const physicsBodyCinfo& bodyCinfo);

    ~physicsBody();

	const std::string& getName() const { return m_name; }
	void setName( const std::string& name ) { m_name = name; }

	unsigned int getBodyId() const { return m_bodyId; }

	// Return read-only access to shape
	inline const physicsShape* getShape() const;

	physicsMotionType getMotionType() const { return m_motionType; }
	inline bool isStatic() const;

	// Read-only access to transforms and motion
	const Vector4& getPosition() const { return m_pos; }
	const Real getRotation() const { return  m_ori; }
	const Vector4& getLinearVelocity() const { return m_linearVelocity; }
	const Real getAngularSpeed() const { return m_angularSpeed; }

	const Real getMass() const { return m_mass; }
	const Real getInertia() const { return m_inertia; }
	const Real getInvMass() const { return m_invMass; }
	const Real getInvInertia() const { return m_invInertia; }

	bool containsPoint( const Vector4& point ) const;

private:

	std::string m_name;
	BodyId m_bodyId;
	std::shared_ptr<physicsShape> m_shape;
	physicsMotionType m_motionType;
	Vector4 m_pos;
	Real m_ori;
	Vector4 m_linearVelocity;
	Real m_angularSpeed; // in radians
	Real m_mass;
	Real m_inertia;

private:

	// These functions are used internally in physicsWorld

	inline void setBodyId( unsigned int bodyId );

	physicsShape::Type getShapeType() const;

	// Internal usage - aabb
	inline physicsAabb getAabb() const;
	void updateAabb();

	// Internal usage - motion type
	inline void setMotionType( physicsMotionType type );

	// Internal usage - position, rotation, linear velocity, angular speed
	inline void setPosition( const Vector4& pos );
	inline void setRotation( const Real rotation );
	inline void setLinearVelocity( const Vector4& linearVel );
	inline void setAngularSpeed( const Real angularVel );

	void getPointVelocity( const Vector4& arm, Vector4& vel ) const; // arm is local
	void setDampedVelocity( const Real& damping );

	// Internal usage - mass, inertia, COM
	inline void setMass(const Real mass);
	inline void setInertia( const Real inertia );

	// Internal usage - indices
	inline void setActiveListIdx( unsigned int idx );
	inline unsigned int getActiveListIdx() const;

	// Internal usage - collision filter
	inline unsigned int getCollisionFilter() const;

	// Used by world to update body from solver bodies
	void setFromSolverBody( const struct SolverBody& body );

private:

	physicsAabb m_aabb;
	Real m_invMass;
	Real m_invInertia;
	unsigned int m_activeListIdx; // Index of this body in physicsWorld::m_activeBodyIds
	unsigned int m_collisionFilter;

	friend class physicsWorld;
	friend class physicsWorldEx;
};

#include <physicsBody.inl>
