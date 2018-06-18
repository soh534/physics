inline const physicsShape* physicsBody::getShape() const
{
	return m_shape.get();
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

inline void physicsBody::setPosition(const Vector4& pos)
{
	m_pos = pos;
}

inline void physicsBody::setRotation(const Real rotation)
{
	m_ori = rotation;
}

inline void physicsBody::setLinearVelocity(const Vector4& linearVel)
{
	m_linearVelocity = linearVel;
}

inline void physicsBody::setAngularSpeed(const Real angularVel)
{
	m_angularSpeed = angularVel;
}

inline void physicsBody::setMass(const Real mass)
{
	m_mass = mass;
	m_invMass = 1.f / m_mass;
}

inline void physicsBody::setInertia(const Real inertia)
{
	m_inertia = inertia;
	m_invInertia = 1.f / m_inertia;
}

inline physicsAabb physicsBody::getAabb() const
{
	return m_aabb;
}

inline void physicsBody::setBodyId(unsigned int bodyId)
{
	m_bodyId = bodyId;
}

inline unsigned int physicsBody::getCollisionFilter() const
{
	return m_collisionFilter;
}

inline void physicsBody::setActiveListIdx( unsigned int idx )
{
	m_activeListIdx = idx;
}

inline unsigned int physicsBody::getActiveListIdx() const
{
	return m_activeListIdx;
}