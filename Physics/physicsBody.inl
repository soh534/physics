inline std::shared_ptr<physicsShape> physicsBody::getShape() const
{
	return m_shape;
}

inline physicsMotionType physicsBody::getMotionType() const
{
	return m_motionType;
}

inline void physicsBody::setMotionType(physicsMotionType type)
{
	m_motionType = type;

	if (m_motionType == physicsMotionType::STATIC)
	{
		m_linearVelocity.setZero();
		m_angularSpeed = 0.f;
	}
}

inline bool physicsBody::isStatic() const
{
	return (m_motionType == physicsMotionType::STATIC);
}

inline const Vector3& physicsBody::getPosition() const
{
	return m_pos;
}

inline Vector3& physicsBody::getPosition()
{
	return m_pos;
}

inline void physicsBody::setPosition(const Vector3& pos)
{
	m_pos = pos;
}

inline const Real physicsBody::getRotation() const
{
	return m_ori;
}

inline Real& physicsBody::getRotation()
{
	return m_ori;
}

inline void physicsBody::setRotation(const Real rotation)
{
	m_ori = rotation;
}

inline const Vector3& physicsBody::getLinearVelocity() const
{
	return m_linearVelocity;
}

inline Vector3& physicsBody::getLinearVelocity()
{
	return m_linearVelocity;
}

inline void physicsBody::setLinearVelocity(const Vector3& linearVel)
{
	m_linearVelocity = linearVel;
}

inline const Vector3& physicsBody::getCenterOfMass() const
{
	return m_com;
}

inline void physicsBody::setCenterOfMass(const Vector3& com)
{
	m_com = com;
}

inline const Real physicsBody::getAngularSpeed() const
{
	return m_angularSpeed;
}

inline Real& physicsBody::getAngularSpeed()
{
	return m_angularSpeed;
}

inline void physicsBody::setAngularSpeed(const Real angularVel)
{
	m_angularSpeed = angularVel;
}

inline const Real physicsBody::getMass() const
{
	return m_mass;
}

inline void physicsBody::setMass(const Real mass)
{
	m_mass = mass;
	m_invMass = 1.f / m_mass;
}

inline const Real physicsBody::getInvMass() const
{
	return m_invMass;
}

inline const Real physicsBody::getInertia() const
{
	return m_inertia;
}

inline void physicsBody::setInertia(const Real inertia)
{
	m_inertia = inertia;
	m_invInertia = 1.f / m_inertia;
}

inline const Real physicsBody::getInvInertia() const
{
	return m_invInertia;
}

inline void physicsBody::setFriction(const Real friction)
{
	m_friction = friction;
}

inline const Real physicsBody::getFriction() const
{
	return m_friction;
}

inline const std::string& physicsBody::getName() const
{
	return m_name;
}

inline void physicsBody::setName(const std::string& name)
{
	m_name = name;
}

inline physicsAabb physicsBody::getAabb() const
{
	return m_aabb;
}

inline void physicsBody::setBodyId(unsigned int bodyId)
{
	m_bodyId = bodyId;
}

inline unsigned int physicsBody::getBodyId() const
{
	return m_bodyId;
}

inline unsigned int physicsBody::getCollisionFilter() const
{
	return m_collisionFilter;
}

inline unsigned int physicsBody::getActiveListIdx() const
{
	return m_activeListIdx;
}