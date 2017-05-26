#include <vector>

#include <Base.h>
#include <physicsObject.h>
#include <physicsInternalTypes.h>
#include <physicsBody.h>
#include <physicsCollider.h>
#include <physicsSolver.h>
#include <physicsWorld.h>

#include <DebugUtils.h>

class physicsWorldEx : public physicsWorld
{
public:

	void registerColliderFunc( physicsShape::Type typeA, physicsShape::Type typeB, ColliderFuncPtr func );

	ColliderFuncPtr getCollisionFunc( const physicsBody& bodyA, const physicsBody& bodyB );

	void collide();

	void collideAabbs( std::vector<BodyIdPair>& broadPhasePassedPairsOut );

	bool checkCollidable( BodyId bodyIdA, BodyId bodyIdB );

	void collideCachedAndNewPairs(
		std::vector<ConstrainedPair>& cachedPairs,
		const std::vector<BodyIdPair>& newPairs );

	void solve();

	void updateJointConstraints();
};

void physicsWorldEx::registerColliderFunc( physicsShape::Type typeA, physicsShape::Type typeB, ColliderFuncPtr func )
{
	m_dispatchTable[typeA][typeB] = func;
	m_dispatchTable[typeB][typeA] = func;
}

ColliderFuncPtr physicsWorldEx::getCollisionFunc( const physicsBody& bodyA, const physicsBody& bodyB )
{
	physicsShape::Type typeA = bodyA.getShapeType();
	physicsShape::Type typeB = bodyB.getShapeType();
	return m_dispatchTable[typeA][typeB];;
}

void physicsWorldEx::collide()
{
	/// Find new pairs in broadphase, cache them, delete caches for lost broadphase pairs

	/// Broadphase
	std::vector<BodyIdPair> bpPassedPairs;
	collideAabbs( bpPassedPairs );
	std::sort( bpPassedPairs.begin(), bpPassedPairs.end(), bodyIdPairLess );

	std::vector<BodyIdPair> bpLostPairs, bpRemainedPairs, bpNewPairs;
	BodyIdPairsUtils::classifyPairSets( m_cachedContactPairs, bpPassedPairs, bpLostPairs, bpRemainedPairs, bpNewPairs );

	/// Remove 
	BodyIdPairsUtils::deletePairsBfromA( m_cachedContactPairs, bpLostPairs );

	collideCachedAndNewPairs( m_cachedContactPairs, bpNewPairs );
}

struct aabbIndex
{
	int m_idx;
	Vector3 m_part;

	aabbIndex( int idx, Vector3 part ) : m_idx( idx ), m_part( part ) {}
};

static bool xless( const aabbIndex& aabbIdx1, const aabbIndex& aabbIdx2 )
{
	return aabbIdx1.m_part( 0 ) < aabbIdx2.m_part( 0 );
}

static bool yless( const aabbIndex& aabbIdx1, const aabbIndex& aabbIdx2 )
{
	return aabbIdx1.m_part( 1 ) < aabbIdx2.m_part( 1 );
}

void physicsWorldEx::collideAabbs( std::vector<BodyIdPair>& broadPhasePassedPairsOut )
{
	// Update AABB's, do 1D sweep & prune, add pairs which have overlapping AABB's
	std::vector<aabbIndex> aabbIndices;

	for ( int i = 0; i < ( int )m_activeBodyIds.size(); i++ )
	{
		int activeBodyId = m_activeBodyIds[i];
		physicsBody& body = m_bodies[activeBodyId];

		body.updateAabb();

		DebugUtils::drawBpAabb( body ); /// TODO: move this out

		physicsAabb aabb = body.getAabb();
		BodyId bodyId = body.getBodyId();
		aabbIndices.push_back( aabbIndex( bodyId, aabb.m_min ) );
		aabbIndices.push_back( aabbIndex( bodyId, aabb.m_max ) );
	}

	std::sort( aabbIndices.begin(), aabbIndices.end(), xless );

	std::vector<bool> flags( m_bodies.size(), false );

	for ( int i = 0; i < ( int )aabbIndices.size(); i++ )
	{
		if ( aabbIndices[i].m_idx == -1 ) continue;

		std::fill( flags.begin(), flags.end(), false );

		for ( int j = i + 1; j < ( int )aabbIndices.size(); j++ )
		{
			if ( aabbIndices[j].m_idx == -1 ) continue;

			if ( aabbIndices[i].m_idx == aabbIndices[j].m_idx )
			{
				aabbIndices[i].m_idx = -1;
				aabbIndices[j].m_idx = -1;
				break;
			}

			if ( flags[aabbIndices[j].m_idx] ) continue;

			const physicsBody& bodyA = m_bodies[aabbIndices[i].m_idx];
			const physicsBody& bodyB = m_bodies[aabbIndices[j].m_idx];

			if ( bodyA.isStatic() && bodyB.isStatic() ) continue;

			if ( !checkCollidable( bodyA.getBodyId(), bodyB.getBodyId() ) ) continue;

			physicsAabb aabbA = bodyA.getAabb();
			physicsAabb aabbB = bodyB.getAabb();

			if ( aabbA.overlaps( aabbB ) )
			{
				BodyIdPair pair( bodyA.getBodyId(), bodyB.getBodyId() );

				broadPhasePassedPairsOut.push_back( pair );

				flags[bodyB.getBodyId()] = true;
			}
		}
	}
}

bool physicsWorldEx::checkCollidable( BodyId bodyIdA, BodyId bodyIdB )
{
	const physicsBody& bodyA = m_bodies[bodyIdA];
	const physicsBody& bodyB = m_bodies[bodyIdB];

	if ( bodyA.getCollisionFilter() == bodyB.getCollisionFilter() )
	{
		return true;
	}

	return false;
}

void setAsContact( Constraint& c, const ContactPoint& contact, const Real rotA, const Real rotB )
{
	c.rA = contact.getContactA();
	c.rB = contact.getContactB();
	c.error = contact.getDepth();

	const Vector3& norm = contact.getNormal();
	const Vector3& rA_ws = c.rA.getRotatedDir( rotA );
	const Vector3& rB_ws = c.rB.getRotatedDir( rotB );

	c.jac.vA = norm;
	c.jac.vB = norm.getNegated();
	c.jac.wA = rA_ws.cross( norm );
	c.jac.wB = rB_ws.cross( norm.getNegated() );
}

void physicsWorldEx::collideCachedAndNewPairs( std::vector<ConstrainedPair>& cachedPairs,
											   const std::vector<BodyIdPair>& newPairs )
{
	auto iterCached = cachedPairs.begin();
	auto iterNew = newPairs.begin();

	while ( true )
	{
		if ( iterCached == cachedPairs.end() && iterNew == newPairs.end() )
		{
			break;
		}

		bool pairIsCached = false;

		if ( iterCached != cachedPairs.end() && iterNew != newPairs.end() )
		{
			pairIsCached = ( *iterCached < *iterNew ) ? true : false;
		}

		if ( iterCached != cachedPairs.end() )
		{
			pairIsCached = true;
		}

		if ( pairIsCached )
		{
			const physicsBody& bodyA = m_bodies[iterCached->bodyIdA];
			const physicsBody& bodyB = m_bodies[iterCached->bodyIdB];
			ColliderFuncPtr colliderFuncPtr = getCollisionFunc( bodyA, bodyB );

			std::vector<ContactPoint> contacts;
			colliderFuncPtr( bodyA, bodyB, contacts );

			setAsContact( iterCached->constraints[0],
						  contacts[0],
						  bodyA.getRotation(), bodyB.getRotation() );

			m_contactSolvePairs.push_back( *iterCached );
			iterCached++;
		}
		else
		{
			const physicsBody& bodyA = m_bodies[iterNew->bodyIdA];
			const physicsBody& bodyB = m_bodies[iterNew->bodyIdB];
			ColliderFuncPtr colliderFuncPtr = getCollisionFunc( bodyA, bodyB );

			std::vector<ContactPoint> contacts;
			colliderFuncPtr( bodyA, bodyB, contacts );

			/// Add new contact constraint
			ConstrainedPair constrainedPair( *iterNew );
			
			Constraint c;
			setAsContact( c, contacts[0], bodyA.getRotation(), bodyB.getRotation() );
			constrainedPair.constraints.push_back( c );

			m_contactSolvePairs.push_back( constrainedPair );

			iterNew++;
		}
	}
}

void physicsWorldEx::solve()
{
	int numActiveBodies = ( int )m_activeBodyIds.size();

	m_solverBodies.resize( numActiveBodies );

	for ( int i = 0; i < numActiveBodies; i++ )
	{
		int activeBodyId = m_activeBodyIds[i];
		physicsBody& body = m_bodies[activeBodyId];

		/// Apply gravity
		if ( !body.isStatic() )
		{
			Vector3& v = body.getLinearVelocity();
			v += m_gravity * getDeltaTime();
		}

		/// Prepare solver bodies
		m_solverBodies[i].setFromBody( body );
	}

	updateJointConstraints();

	/// Solve constraints
	m_solver->solveConstraints( m_solverInfo, m_contactSolvePairs, m_solverBodies, m_bodies );
	m_solver->solveConstraints( m_solverInfo, m_jointSolvePairs, m_solverBodies, m_bodies );

	/// Integrate time
	for ( int i = 0; i < ( int )m_activeBodyIds.size(); i++ )
	{
		int activeBodyId = m_activeBodyIds[i];
		physicsBody& body = m_bodies[activeBodyId];

		if ( !body.isStatic() )
		{
			const Vector3& v = body.getLinearVelocity();
			Vector3& p = body.getPosition();
			p += v * m_solverInfo.m_deltaTime;

			const Real& w = body.getAngularSpeed();
			Real& rot = body.getRotation();
			rot += w * m_solverInfo.m_deltaTime;

			body.updateAabb(); /// TODO: this just for debug display, remove later
		}
	}
}

void physicsWorldEx::updateJointConstraints()
{
	auto iterJoint = m_jointSolvePairs.begin();

	while ( iterJoint != m_jointSolvePairs.end() )
	{
		const physicsBody& bodyA = m_bodies[iterJoint->bodyIdA];
		const physicsBody& bodyB = m_bodies[iterJoint->bodyIdB];

		const Real rotA = bodyA.getRotation();
		const Real rotB = bodyB.getRotation();
		const Vector3& posA = bodyA.getPosition();
		const Vector3& posB = bodyB.getPosition();

		Constraint& constraintX = iterJoint->constraints[0];
		const Vector3& rAworldx = constraintX.rA.getRotatedDir( rotA );
		const Vector3& rBworldx = constraintX.rB.getRotatedDir( rotB );

		constraintX.error = -( posA + rAworldx - posB - rBworldx )( 0 );
		constraintX.jac.wA = rAworldx.cross( constraintX.jac.vA );
		constraintX.jac.wB = rBworldx.cross( constraintX.jac.vB );

		Constraint& constraintY = iterJoint->constraints[1];
		const Vector3& rAworldy = constraintY.rA.getRotatedDir( rotA );
		const Vector3& rBworldy = constraintY.rB.getRotatedDir( rotB );

		constraintY.error = -( posA + rAworldy - posB - rBworldy )( 1 );
		constraintY.jac.wA = rAworldy.cross( constraintY.jac.vA );
		constraintY.jac.wB = rBworldy.cross( constraintY.jac.vB );

		iterJoint++;
	}
}

physicsWorld::physicsWorld( const physicsWorldCinfo& cinfo ) :
	m_gravity( cinfo.m_gravity ),
	m_cor( cinfo.m_cor ),
	m_firstFreeBodyId( 0 )
{
	m_solver = new physicsSolver;

	m_solverInfo.m_deltaTime = 0.16f;
	m_solverInfo.m_numIter = 1;

	physicsWorldEx* self = static_cast<physicsWorldEx*>( this );

	self->registerColliderFunc( physicsShape::BASE, physicsShape::BASE, nullptr );
	self->registerColliderFunc( physicsShape::BASE, physicsShape::CIRCLE, nullptr );
	self->registerColliderFunc( physicsShape::BASE, physicsShape::BOX, nullptr );
	self->registerColliderFunc( physicsShape::BASE, physicsShape::CONVEX, nullptr );
	self->registerColliderFunc( physicsShape::CIRCLE, physicsShape::CIRCLE, physicsCircleCollider::collide );
	self->registerColliderFunc( physicsShape::CIRCLE, physicsShape::BOX, physicsCircleBoxCollider::collide );
	self->registerColliderFunc( physicsShape::CIRCLE, physicsShape::CONVEX, physicsConvexCollider::collide );
	self->registerColliderFunc( physicsShape::BOX, physicsShape::BOX, physicsBoxCollider::collide );
	self->registerColliderFunc( physicsShape::BOX, physicsShape::CONVEX, physicsConvexCollider::collide );
	self->registerColliderFunc( physicsShape::CONVEX, physicsShape::CONVEX, physicsConvexCollider::collide );
}

physicsWorld::~physicsWorld()
{
	delete m_solver;
	m_bodies.clear();
}

BodyId physicsWorld::createBody( const physicsBodyCinfo& cinfo )
{
	if ( m_firstFreeBodyId < m_bodies.size() )
	{
		/// Re-use previously allocated space for removed body
		unsigned int freeId = m_firstFreeBodyId;
		physicsBody& body = m_bodies[freeId];
		FreeBody& freeBody = *reinterpret_cast<FreeBody*>( &body );
		m_firstFreeBodyId = freeBody.m_nextFreeBodyIdx;
		body = physicsBody( cinfo );

		body.setBodyId( freeId );
		m_activeBodyIds.push_back( body.getBodyId() );

		return body.getBodyId();
	}
	else
	{
		/// Append on back
		m_bodies.push_back( physicsBody( cinfo ) );
		m_firstFreeBodyId = static_cast< BodyId >( m_bodies.size() );
		physicsBody& body = m_bodies.back();

		body.setBodyId( m_bodies.size() - 1 );
		m_activeBodyIds.push_back( body.getBodyId() );

		return body.getBodyId();
	}
}

void physicsWorld::removeBody( const BodyId bodyId )
{
	/// Body removed locations will be re-used for future body additions
	physicsBody& body = m_bodies[bodyId];

	body.getShape().reset();

	/// Remove bodyId from actively simulated set
	int activeListIdx = body.getActiveListIdx();
	std::swap( m_activeBodyIds[activeListIdx], m_activeBodyIds.back() );
	m_activeBodyIds.pop_back();

	/// Add removed body's index to free body linked list
	FreeBody& freeBody = *reinterpret_cast<FreeBody*>( &body );
	freeBody.m_nextFreeBodyIdx = m_firstFreeBodyId;
	m_firstFreeBodyId = body.getBodyId();
}

const physicsBody& physicsWorld::getBody( const BodyId bodyId ) const
{
	/// TODO: add O(1) check which checks body is active
	return m_bodies[bodyId];
}

int physicsWorld::addJoint( const JointCinfo& config )
{
	ConstrainedPair joint(config.bodyIdA, config.bodyIdB);
	{
		const physicsBody& bodyA = m_bodies[joint.bodyIdA];
		const physicsBody& bodyB = m_bodies[joint.bodyIdB];

		Vector3 rAworld = config.pivot - bodyA.getPosition();
		Vector3 rBworld = config.pivot - bodyB.getPosition();

		Vector3 rAlocal = rAworld.getRotatedDir( -bodyA.getRotation() );
		Vector3 rBlocal = rBworld.getRotatedDir( -bodyB.getRotation() );

		/// Constraint x-axis
		{
			Constraint constraintX;
			constraintX.rA = rAlocal;
			constraintX.rB = rBlocal;
			constraintX.accumImp = 0.f;
			constraintX.error = 0.f;
			constraintX.jac.vA.set( 1.f, 0.f );
			constraintX.jac.vB.set( -1.f, 0.f );
			constraintX.jac.wA = constraintX.rA.cross( constraintX.jac.vA );
			constraintX.jac.wB = constraintX.rB.cross( constraintX.jac.vB );

			joint.constraints.push_back( constraintX );
		}

		/// Constraint y-axis
		{ 
			Constraint constraintY;
			constraintY.rA = rAlocal;
			constraintY.rB = rBlocal;
			constraintY.accumImp = 0.f;
			constraintY.error = 0.f;
			constraintY.jac.vA.set( 0.f, 1.f );
			constraintY.jac.vB.set( 0.f, -1.f );
			constraintY.jac.wA = constraintY.rA.cross( constraintY.jac.vA );
			constraintY.jac.wB = constraintY.rB.cross( constraintY.jac.vB );

			joint.constraints.push_back( constraintY );
		}
	}

	m_jointSolvePairs.push_back( joint );
	return ( m_jointSolvePairs.size() - 1 );
}

void physicsWorld::removeJoint( JointId jointId )
{
	/// TODO: think about what to do with this
	auto jiter = m_jointSolvePairs.begin();
	m_jointSolvePairs.erase( jiter + jointId );
}

void physicsWorld::step()
{
	physicsWorldEx* self = static_cast<physicsWorldEx*>( this );
	self->collide();
	self->solve();
}

/// TODO: move this function outside of physics
void physicsWorld::render()
{
	for (size_t i = 0; i < m_activeBodyIds.size(); i++)
	{
		int activeBodyId = m_activeBodyIds[i];
		const physicsBody& body = m_bodies[activeBodyId];
		body.render();
	}
}

/// TODO: prevent removed bodyId input
void physicsWorld::setPosition( BodyId bodyId, const Vector3& point )
{
	physicsBody& body = m_bodies[bodyId];
	body.setPosition( point );
}

void physicsWorld::setMotionType( BodyId bodyId, physicsMotionType type )
{
	physicsBody& body = m_bodies[bodyId];
	body.setMotionType( type );
}

const Real physicsWorld::getDeltaTime()
{
	return m_solverInfo.m_deltaTime;
}
