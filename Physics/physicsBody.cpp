#include <Base.h>

#include <physicsTypes.h>
#include <physicsBody.h>
#include <physicsObject.h>
#include <physicsSolver.h>

#include <Renderer.h>

#include <sstream>

physicsBodyCinfo::physicsBodyCinfo()
{
	// Default values if no settings are specified
	m_name = "";
	m_shape = nullptr;
	m_motionType = physicsMotionType::DYNAMIC;
	m_pos.setZero();
	m_ori = 0.f;
	m_linearVelocity.setZero();
	m_angularSpeed = 0.f;
	m_mass = -1.f;
	m_inertia = -1.f;
	m_collidable = true;
}

physicsBodyCinfo::~physicsBodyCinfo()
{

}

physicsBody::physicsBody( const physicsBodyCinfo& bodyCinfo ) :
	m_name( bodyCinfo.m_name ),
	m_bodyId( invalidId ),
	m_shape( bodyCinfo.m_shape ),
	m_motionType( bodyCinfo.m_motionType ),
	m_pos( bodyCinfo.m_pos ),
	m_ori( bodyCinfo.m_ori ),
	m_linearVelocity( bodyCinfo.m_linearVelocity ),
	m_angularSpeed( bodyCinfo.m_angularSpeed ),
	m_mass( bodyCinfo.m_mass ),
	m_inertia( bodyCinfo.m_inertia )
{

	if ( bodyCinfo.m_motionType == physicsMotionType::DYNAMIC )
	{
		// Set up mass and inertia if not set (recommended way)
		if ( bodyCinfo.m_mass < 0.f )
		{
			m_mass = m_shape->calculateMass();
		}

		if ( bodyCinfo.m_inertia < 0.f )
		{
			m_inertia = m_shape->calculateInertia();
		}

		m_invMass = 1.f / m_mass;
		m_invInertia = 1.f / m_inertia;
	}
	else if ( bodyCinfo.m_motionType == physicsMotionType::STATIC )
	{
		Assert( m_mass < 0.f, "Mass shouldn't have been set for static bodies." );
		Assert( m_inertia < 0.f, "Inertia shouldn't have been set for static bodies" );
		m_invMass = 0.f;
		m_invInertia = 0.f;
	}
	else
	{
		Assert( false, "Trying to construct invalid body type." );
	}

	m_collisionFilter = ( bodyCinfo.m_collidable ) ? 0 : 1;
}

physicsBody::~physicsBody()
{

}

bool physicsBody::containsPoint( const Vector4& point ) const
{
	// Convert point: world->local
	Vector4 local;
	local.setSub( point, m_pos );
	local.setRotatedDir( local, -m_ori );

	return m_shape->containsPoint( local );
}

//#define PREDICTIVE_C
void physicsBody::updateAabb()
{
	m_aabb = m_shape->getAabb( m_ori );
#if defined PREDICTIVE_C
	m_aabb.expand( m_linearVelocity );
#else
	m_aabb.expand( 0.5f );
#endif
	m_aabb.translate( m_pos );
}

void physicsBody::getPointVelocity( const Vector4& arm, Vector4& vel ) const
{
	// TODO: Test
	Vector4 w( 0.f, 0.f, m_angularSpeed );
	Vector4 tangentVel = w.cross( arm.getRotatedDir( m_ori ) );
	vel = tangentVel + m_linearVelocity;
}

void physicsBody::setDampedVelocity( const Real& damping )
{
	// Example of damping would be 0.95f;
	setLinearVelocity( getLinearVelocity() * damping );
	setAngularSpeed( getAngularSpeed() * damping );
}

physicsShape::Type physicsBody::getShapeType() const
{
	return m_shape->getType();
}

void physicsBody::setFromSolverBody( const SolverBody& body )
{
	setLinearVelocity( body.v );
	setAngularSpeed( body.w( 2 ) );
}