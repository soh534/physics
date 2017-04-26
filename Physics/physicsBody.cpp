#include <Base.hpp>

#include <physicsBody.hpp>
#include <physicsObject.hpp>

#include <Renderer.hpp>

physicsBodyCinfo::physicsBodyCinfo()
{ 
	/// Default values if no settings are specified
    m_shape = nullptr;
	m_type = physicsMotionType::DYNAMIC;
    m_pos.set(0.0f, 0.0f);
    m_ori = 0.0f;
	m_com = m_pos;
    m_linearVelocity.set(0.0f, 0.0f);
    m_angularSpeed = 0.0f;
    m_mass = -1.0f;
	m_inertia = -1.0f;
	m_friction = 0.5f;
	m_collidable = true;
}

physicsBody::physicsBody(const physicsBodyCinfo& bodyCinfo) :
	m_shape(bodyCinfo.m_shape),
	m_type(bodyCinfo.m_type),
	m_pos(bodyCinfo.m_pos),
	m_ori(bodyCinfo.m_ori),
	m_com(bodyCinfo.m_com),
	m_linearVelocity(bodyCinfo.m_linearVelocity),
	m_angularSpeed(bodyCinfo.m_angularSpeed),
	m_mass(bodyCinfo.m_mass),
	m_inertia(bodyCinfo.m_inertia),
	m_friction(bodyCinfo.m_friction),
	m_bodyId(-1)
{

	if (bodyCinfo.m_type == physicsMotionType::DYNAMIC)
	{
		/// Set up mass and inertia if not set (recommended way)
		if (bodyCinfo.m_mass < 0.f)
		{
			m_mass = m_shape->calculateMass();
		}

		if (bodyCinfo.m_inertia < 0.f)
		{
			m_inertia = m_shape->calculateInertia();
		}

		m_invMass = 1.f / m_mass;
		m_invInertia = 1.f / m_inertia;
	}
	else if (bodyCinfo.m_type == physicsMotionType::STATIC)
	{
		Assert(m_mass < 0.f, "Mass shouldn't have been set for static bodies.");
		Assert(m_inertia < 0.f, "Inertia shouldn't have been set for static bodies");
		m_invMass = 0.f;
		m_invInertia = 0.f;
	}
	else
	{
		Assert(false, "Trying to construct invalid body type.");
	}

	if (bodyCinfo.m_collidable)
	{
		m_bodyFilter = 0;
	}
	else
	{
		m_bodyFilter = 1;
	}
}

physicsBody::~physicsBody()
{
    delete m_shape;
}

void physicsBody::render() const
{ 
	/// TODO: move this function outside of physics

	// Mark central point and lines constructing body's shape
	drawArrow( m_pos, Vector3( 0.f, 10.f ).getRotatedDir( m_ori ) );

	Vector3 offset( 5.f, 5.f );
	drawText( std::to_string( m_bodyId ), m_pos + offset );
    m_shape->render(m_pos, m_ori);
}

bool physicsBody::containsPoint(const Vector3& point) const
{
	// Convert point from global to body local
	Vector3 local;
	local.setSub(point, m_pos);
	local.setRotatedDir(local, -m_ori);

    return m_shape->containsPoint(local);
}

void physicsBody::updateAabb()
{
    m_aabb = m_shape->getAabb(m_ori);
    m_aabb.expand(0.5f);
    m_aabb.translate(m_pos);
}

void physicsBody::getPointVelocity(const Vector3& arm, Vector3& vel) const
{
	// TODO: Test
	Vector3 w(0.f, 0.f, m_angularSpeed);
	Vector3 tangentVel = w.cross(arm.getRotatedDir(m_ori));
	vel = tangentVel + m_linearVelocity;
	if (vel.length() > 400.f)
	{
		//vel.setZero();
	}
}

void physicsBody::dampVelocity(const Real& damping)
{
	/// Example of damping would be 0.95f;
	setLinearVelocity(getLinearVelocity() * damping);
	setAngularSpeed(getAngularSpeed() * damping);
}

#include <sstream>

void physicsBody::applyPointImpulse(const Vector3& impulse, const Vector3& arm)
{
	Vector3 linVel = m_linearVelocity;
	Real angSpeed = m_angularSpeed;

	linVel += impulse * m_invMass;

	Vector3 angImp = arm.cross(impulse);
	angSpeed += (angImp * m_invInertia)(2);

	setLinearVelocity(linVel);
	setAngularSpeed(angSpeed);

	std::stringstream ss;
	ss.precision(3);
	ss << angSpeed << std::endl;
	
	drawText(ss.str(), Vector3(200.f, 200.f));
}

void physicsBody::getInvInertiaWorld(Matrix3& invInertiaWorld) const
{
	Matrix3 rot, rott;
	rot.setRotation(m_ori);
	rott.setTranspose(rot);

	Matrix3 invInertia, temp;
	invInertia.setDiagonalMatrix(0., 0., m_invInertia);
	temp.setMul(rot, invInertia);
	invInertiaWorld.setMul(temp, rott);
}

void physicsBody::calcEffectiveMassMatrixAt(const Vector3& ptWorld, Matrix3& effMassMatrix) const
{
	Vector3 r = ptWorld - m_pos;
	Matrix3 rhat; rhat.setCrossMatrix(r);

	Matrix3 invMass; invMass.setDiagonalMatrix(m_invMass, m_invMass, m_invMass);

	Matrix3 invInertiaWorld; getInvInertiaWorld(invInertiaWorld);
	Matrix3 temp0, temp1;
	temp0.setMul(rhat, invInertiaWorld);
	temp1.setMul(temp0, rhat);
	
	temp0.setSub(invMass, temp1);

	effMassMatrix.setInverse22(temp0);
}

physicsShape::Type physicsBody::getShapeType() const
{
	return m_shape->getType();
}