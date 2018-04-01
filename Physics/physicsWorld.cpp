#include <vector>

#include <Base.h>
#include <physicsObject.h>
#include <physicsInternalTypes.h>
#include <physicsBody.h>
#include <physicsCollider.h>
#include <physicsSolver.h>
#include <physicsWorld.h>

#include <DebugUtils.h>

// TODO: separate physics as library from the framework
class physicsWorldEx : public physicsWorld
{
public:

	void registerColliderFunc( physicsShape::Type typeA, physicsShape::Type typeB, ColliderFuncPtr func )
	{
		m_dispatchTable[typeA][typeB] = func;
		m_dispatchTable[typeB][typeA] = func;
	}

	ColliderFuncPtr getCollisionFunc( const physicsBody& bodyA, const physicsBody& bodyB )
	{
		physicsShape::Type typeA = bodyA.getShapeType();
		physicsShape::Type typeB = bodyB.getShapeType();
		return m_dispatchTable[typeA][typeB];;
	}

	void collide();

	// Accept array of indexed AABB's, return pairs which overlap
	void collideAabbs( const std::vector<BroadphaseBody>& broadphaseBodies,
		std::vector<BodyIdPair>& broadPhasePassedPairsOut );

	bool checkCollidable( BodyId bodyIdA, BodyId bodyIdB )
	{
		const physicsBody& bodyA = m_bodies[bodyIdA];
		const physicsBody& bodyB = m_bodies[bodyIdB];

		if ( bodyA.getCollisionFilter() == bodyB.getCollisionFilter() )
		{
			return true;
		}

		return false;
	}

	void mergeCollidableStreams(
		const std::vector<BodyIdPair>& cachedPairs,
		const std::vector<BodyIdPair>& newPairs );

	void solve();

	void updateJointConstraints();
};

void physicsWorldEx::collide()
{
	// Find new pairs in broadphase, delete caches for lost broadphase pairs

	// Update broadphase AABB's
	m_broadphaseBodies.clear(); // TODO: don't clear this, just overwrite the contents
	for ( auto i = 0; i < m_activeBodyIds.size(); i++ )
	{
		int activeBodyId = m_activeBodyIds[i];
		physicsBody& body = m_bodies[activeBodyId];

		body.updateAabb(); // TODO: only update if aabb is dirty, don't update for static bodies

		BroadphaseBody bpBody( body.getBodyId(), body.getAabb() );
		m_broadphaseBodies.push_back( bpBody ); // TODO: don't push_back this, just overwrite the contents
	}

	std::vector<BodyIdPair> bpPassedPairs;
	collideAabbs( m_broadphaseBodies, bpPassedPairs );

	std::sort( bpPassedPairs.begin(), bpPassedPairs.end(), bodyIdPairLess );
	std::sort( m_existingPairs.begin(), m_existingPairs.end(), bodyIdPairLess );
	std::vector<BodyIdPair> bpLostPairs, bpRemainedPairs;
	BodyIdPairsUtils::classifyPairSets( m_existingPairs, bpPassedPairs, bpLostPairs, bpRemainedPairs, m_newPairs );

	// Remove collision caches for which we lose broadphase pair
	BodyIdPairsUtils::deletePairsBfromA( m_cachedPairs, bpLostPairs );
	BodyIdPairsUtils::deletePairsBfromA( m_existingPairs, bpLostPairs );

	mergeCollidableStreams( m_existingPairs, m_newPairs );

	BodyIdPairsUtils::movePairsBtoA( m_existingPairs, m_newPairs );
}

struct aabbIndex
{
	int m_idx;
	Vector4 m_part;

	aabbIndex( int idx, const Vector4& part ) : m_idx( idx ), m_part( part ) {}
};

static bool xless( const aabbIndex& aabbIdx1, const aabbIndex& aabbIdx2 )
{
	return aabbIdx1.m_part( 0 ) < aabbIdx2.m_part( 0 );
}

static bool yless( const aabbIndex& aabbIdx1, const aabbIndex& aabbIdx2 )
{
	return aabbIdx1.m_part( 1 ) < aabbIdx2.m_part( 1 );
}

void physicsWorldEx::collideAabbs( const std::vector<BroadphaseBody>& broadphaseBodies,
								   std::vector<BodyIdPair>& broadPhasePassedPairsOut )
{
	// Do 1D sweep & prune, add pairs which have overlapping AABB's
	std::vector<aabbIndex> aabbIndices; // TODO: don't keep reconstructing this vector, store it and re-use

	int numBpBodies = ( int )broadphaseBodies.size();

	for ( int i = 0; i < numBpBodies; i++ )
	{
		const BroadphaseBody& bpBody = broadphaseBodies[i];

		aabbIndex idxMin( bpBody.bodyId, bpBody.aabb.m_min );
		aabbIndex idxMax( bpBody.bodyId, bpBody.aabb.m_max );
		aabbIndices.push_back( idxMin );
		aabbIndices.push_back( idxMax );
	}

	std::sort( aabbIndices.begin(), aabbIndices.end(), xless );

	std::vector<bool> flags( numBpBodies, false );

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

void setAsContact( Constraint& constraint, const ContactPoint& contact, const Real rotA, const Real rotB )
{
	constraint.rA = contact.getContactA();
	constraint.rB = contact.getContactB();
	constraint.error = contact.getDepth();

	Vector4 norm = contact.getNormal(); norm.normalize<2>();
	const Vector4& rA_ws = constraint.rA.getRotatedDir( rotA );
	const Vector4& rB_ws = constraint.rB.getRotatedDir( rotB );

	constraint.jac.vA = norm.getNegated();
	constraint.jac.vB = norm;
	constraint.jac.wA = rA_ws.cross( constraint.jac.vA );
	constraint.jac.wB = rB_ws.cross( constraint.jac.vB );
}

void setAsFriction( Constraint& constraint, const ContactPoint& contact, const Real rotA, const Real rotB )
{
	constraint.rA = contact.getContactA();
	constraint.rB = contact.getContactB();
	constraint.error = 0.f;

	Transform tangential; tangential.setRotation( 90.f * g_degToRad );
	Transform tA; tA.setRotation( rotA );
	Transform tB; tB.setRotation( rotB );

	Vector4 norm = contact.getNormal(); norm.normalize<2>();
	Vector4 rA_ws; rA_ws.setTransformedPos( tA, constraint.rA );
	Vector4 rB_ws; rB_ws.setTransformedPos( tB, constraint.rB );

	constraint.jac.vA.setTransformedPos( tangential, norm.getNegated() );
	constraint.jac.vB = constraint.jac.vA.getNegated();
	constraint.jac.wA = rA_ws.cross( constraint.jac.vA );
	constraint.jac.wB = rB_ws.cross( constraint.jac.vB );
}

void physicsWorldEx::mergeCollidableStreams( const std::vector<BodyIdPair>& existingPairs,
											 const std::vector<BodyIdPair>& newPairs )
{
	auto iterExisting = existingPairs.begin();
	auto iterNew = newPairs.begin();
	auto iterCached = m_cachedPairs.begin();

	std::vector<CachedPair> pairsCachedThisFrame;

	while ( true )
	{
		if ( iterExisting == existingPairs.end() && iterNew == newPairs.end() )
		{
			break;
		}

		BodyIdPair currentPair;

		if ( iterExisting != existingPairs.end() && iterNew != newPairs.end() )
		{
			Assert( *iterExisting != *iterNew, "can't have same pairs both existing and new" );

			if ( *iterExisting < *iterNew )
			{
				currentPair = *iterExisting;
				iterExisting++;
			}
			else
			{
				currentPair = *iterNew;
				iterNew++;
			}
		}
		else
		{
			if ( iterExisting != existingPairs.end() )
			{
				currentPair = *iterExisting;
				iterExisting++;
			}
			else
			{
				currentPair = *iterNew;
				iterNew++;
			}
		}

		const physicsBody& bodyA = m_bodies[currentPair.bodyIdA];
		const physicsBody& bodyB = m_bodies[currentPair.bodyIdB];
		Transform transformA( bodyA.getPosition(), bodyA.getRotation() );
		Transform transformB( bodyB.getPosition(), bodyB.getRotation() );

		ColliderFuncPtr colliderFuncPtr = getCollisionFunc( bodyA, bodyB );

		std::vector<ContactPoint> contacts;
		colliderFuncPtr( bodyA.getShape(), bodyB.getShape(), transformA, transformB, contacts );

		bool canUseCache = false;

		if ( iterCached != m_cachedPairs.end() )
		{
			if ( currentPair == *iterCached )
			{
				canUseCache = true;

				// Clean up cache
				
			}
		}

		if ( canUseCache )
		{
			if ( contacts.size() > 0 )
			{
				iterCached->addContact( contacts[0] );

				// Add new contact constraint
				ConstrainedPair constrainedPair( currentPair );

				constrainedPair.accumImp = iterCached->accumImp;  // re-use impulse
				//drawText( std::to_string( constrainedPair.accumImp ), Vector3( 50.f, 50.f ) );

				Constraint contactA;
				//setAsContact( constraint0, iterCached->cpA, bodyA.getRotation(), bodyB.getRotation() );
				setAsContact( contactA, contacts[0], bodyA.getRotation(), bodyB.getRotation() );
				constrainedPair.constraints.push_back( contactA );

				Constraint frictionA;
				setAsFriction( frictionA, contacts[0], bodyA.getRotation(), bodyB.getRotation() );
				constrainedPair.constraints.push_back( frictionA );

				if ( false )
				//if ( iterCached->numContacts == 2 )
				{
					Constraint constraint1;
					setAsContact( constraint1, iterCached->cpB, bodyA.getRotation(), bodyB.getRotation() );
					constrainedPair.constraints.push_back( constraint1 );
				}

				m_contactSolvePairs.push_back( constrainedPair );
			}

			iterCached++;
		}
		else
		{
			if ( contacts.size() > 0 )
			{
				CachedPair cachedPair( currentPair );
				cachedPair.addContact( contacts[0] );
				pairsCachedThisFrame.push_back( cachedPair );

				// Add new contact constraint
				ConstrainedPair constrainedPair( currentPair );

				Constraint constraint;
				setAsContact( constraint, contacts[0], bodyA.getRotation(), bodyB.getRotation() );
				constrainedPair.constraints.push_back( constraint );

				m_contactSolvePairs.push_back( constrainedPair );
			}
		}
	}

	BodyIdPairsUtils::movePairsBtoA( m_cachedPairs, pairsCachedThisFrame );
	std::sort( m_cachedPairs.begin(), m_cachedPairs.end(), bodyIdPairLess );
}

void physicsWorldEx::solve()
{
	int numActiveBodies = ( int )m_activeBodyIds.size();

	m_solverBodies.resize( numActiveBodies );

	for ( int i = 0; i < numActiveBodies; i++ )
	{
		int activeBodyId = m_activeBodyIds[i];
		physicsBody& body = m_bodies[activeBodyId];

		// Apply gravity
		if ( !body.isStatic() )
		{
			const Vector4& currLinVel = body.getLinearVelocity();
			body.setLinearVelocity( currLinVel + m_gravity * m_solverInfo.m_deltaTime );
		}

		// Prepare solver bodies
		m_solverBodies[i].setFromBody( body );
	}

	updateJointConstraints();

	// Solve constraints
	m_solver->solveConstraints( m_solverInfo, true, m_contactSolvePairs, m_solverBodies );
	m_solver->solveConstraints( m_solverInfo, false, m_jointSolvePairs, m_solverBodies );

	// Store contact impulses
	{
		auto contactIter = m_contactSolvePairs.begin();
		auto cacheIter = m_cachedPairs.begin();

		while ( contactIter != m_contactSolvePairs.end() )
		{
			if ( *contactIter == *cacheIter )
			{
				cacheIter->accumImp = contactIter->accumImp;
				contactIter++;
			}

			cacheIter++;
		}
	}

	m_contactSolvePairs.clear();

	// Update body velocities
	for ( int activeBodyIdsIdx = 0; activeBodyIdsIdx < ( int ) m_solverBodies.size(); activeBodyIdsIdx++ )
	{
		const SolverBody& solverBody = m_solverBodies[activeBodyIdsIdx];
		physicsBody& body = m_bodies[activeBodyIdsIdx];
		body.setFromSolverBody( solverBody );
	}

	// Integrate time
	for ( int i = 0; i < ( int )m_activeBodyIds.size(); i++ )
	{
		int activeBodyId = m_activeBodyIds[i];
		physicsBody& body = m_bodies[activeBodyId];

		if ( !body.isStatic() )
		{
			const Vector4& linVel = body.getLinearVelocity();
			const Vector4& pos = body.getPosition();
			body.setPosition( pos + linVel * m_solverInfo.m_deltaTime );

			const Real& w = body.getAngularSpeed();
			const Real& rot = body.getRotation();
			body.setRotation( rot + w * m_solverInfo.m_deltaTime );
		}
	}
}

void physicsWorldEx::updateJointConstraints()
{
	for ( auto iterJoint = m_jointSolvePairs.begin(); iterJoint != m_jointSolvePairs.end(); iterJoint++ )
	{
		const physicsBody& bodyA = m_bodies[iterJoint->bodyIdA];
		const physicsBody& bodyB = m_bodies[iterJoint->bodyIdB];

		const Real rotA = bodyA.getRotation();
		const Real rotB = bodyB.getRotation();
		const Vector4& posA = bodyA.getPosition();
		const Vector4& posB = bodyB.getPosition();

		Constraint& constraintX = iterJoint->constraints[0];
		const Vector4& rAworldx = constraintX.rA.getRotatedDir( rotA );
		const Vector4& rBworldx = constraintX.rB.getRotatedDir( rotB );

		constraintX.error = -( posA + rAworldx - posB - rBworldx )( 0 );
		constraintX.jac.wA = rAworldx.cross( constraintX.jac.vA );
		constraintX.jac.wB = rBworldx.cross( constraintX.jac.vB );

		Constraint& constraintY = iterJoint->constraints[1];
		const Vector4& rAworldy = constraintY.rA.getRotatedDir( rotA );
		const Vector4& rBworldy = constraintY.rB.getRotatedDir( rotB );

		constraintY.error = -( posA + rAworldy - posB - rBworldy )( 1 );
		constraintY.jac.wA = rAworldy.cross( constraintY.jac.vA );
		constraintY.jac.wB = rBworldy.cross( constraintY.jac.vB );
	}
}

physicsWorld::physicsWorld( const physicsWorldConfig& cinfo ) :
	m_gravity( cinfo.m_gravity ),
	m_cor( cinfo.m_cor ),
	m_firstFreeBodyId( 0 )
{
	m_solver = new physicsSolver;

	m_solverInfo.m_deltaTime = cinfo.m_deltaTime;
	m_solverInfo.m_numIter = cinfo.m_numIter;

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
		// Re-use previously allocated space for removed body
		unsigned int freeId = m_firstFreeBodyId;
		physicsBody& body = m_bodies[freeId];
		FreeBody& freeBody = *reinterpret_cast<FreeBody*>( &body );
		m_firstFreeBodyId = freeBody.m_nextFreeBodyIdx;
		body = physicsBody( cinfo );

		body.setBodyId( freeId );
		m_activeBodyIds.push_back( body.getBodyId() );
		body.setActiveListIdx( static_cast< int >( m_activeBodyIds.size() ) - 1 );

		return body.getBodyId();
	}
	else
	{
		// Append on back
		m_bodies.push_back( physicsBody( cinfo ) );
		m_firstFreeBodyId = static_cast< BodyId >( m_bodies.size() );
		physicsBody& body = m_bodies.back();

		body.setBodyId( static_cast< int >( m_bodies.size() ) - 1 );
		m_activeBodyIds.push_back( body.getBodyId() );
		body.setActiveListIdx( static_cast< int >( m_activeBodyIds.size() ) - 1 );

		return body.getBodyId();
	}
}

void physicsWorld::removeBody( const BodyId bodyId )
{
	// Body removed locations will be re-used for future body additions
	physicsBody& body = m_bodies[bodyId];



	// Remove bodyId from actively simulated set
	int activeListIdx = body.getActiveListIdx();
	std::swap( m_activeBodyIds[activeListIdx], m_activeBodyIds.back() );
	m_activeBodyIds.pop_back();

	// Add removed body's index to free body linked list
	FreeBody& freeBody = *reinterpret_cast<FreeBody*>( &body );
	freeBody.m_nextFreeBodyIdx = m_firstFreeBodyId;
	m_firstFreeBodyId = body.getBodyId();
}

int physicsWorld::addJoint( const JointConfig& config )
{
	ConstrainedPair joint( config.bodyIdA, config.bodyIdB );
	{
		const physicsBody& bodyA = m_bodies[joint.bodyIdA];
		const physicsBody& bodyB = m_bodies[joint.bodyIdB];

		Vector4 rAworld = config.pivot - bodyA.getPosition();
		Vector4 rBworld = config.pivot - bodyB.getPosition();

		Vector4 rAlocal = rAworld.getRotatedDir( -bodyA.getRotation() );
		Vector4 rBlocal = rBworld.getRotatedDir( -bodyB.getRotation() );

		// Constraint x-axis
		{
			Constraint constraintX;
			constraintX.rA = rAlocal;
			constraintX.rB = rBlocal;
			constraintX.error = 0.f;
			constraintX.jac.vA.set( 1.f, 0.f );
			constraintX.jac.vB.set( -1.f, 0.f );
			constraintX.jac.wA = constraintX.rA.cross( constraintX.jac.vA );
			constraintX.jac.wB = constraintX.rB.cross( constraintX.jac.vB );

			joint.constraints.push_back( constraintX );
		}

		// Constraint y-axis
		{
			Constraint constraintY;
			constraintY.rA = rAlocal;
			constraintY.rB = rBlocal;
			constraintY.error = 0.f;
			constraintY.jac.vA.set( 0.f, 1.f );
			constraintY.jac.vB.set( 0.f, -1.f );
			constraintY.jac.wA = constraintY.rA.cross( constraintY.jac.vA );
			constraintY.jac.wB = constraintY.rB.cross( constraintY.jac.vB );

			joint.constraints.push_back( constraintY );
		}
	}

	m_jointSolvePairs.push_back( joint );
	return ( static_cast< int >( m_jointSolvePairs.size() ) - 1 );
}

void physicsWorld::removeJoint( JointId jointId )
{
	// TODO: think about what to do with this
	auto jiter = m_jointSolvePairs.begin();
	m_jointSolvePairs.erase( jiter + jointId );
}

void physicsWorld::step()
{
	physicsWorldEx* self = static_cast<physicsWorldEx*>( this );
	self->collide();
	self->solve();
}

// TODO: prevent removed bodyId input
void physicsWorld::setPosition( BodyId bodyId, const Vector4& point )
{
	physicsBody& body = m_bodies[bodyId];
	body.setPosition( point );
}

physicsMotionType physicsWorld::getMotionType( BodyId bodyId ) const
{
	return getBody( bodyId ).getMotionType();
}

void physicsWorld::setMotionType( BodyId bodyId, physicsMotionType type )
{
	physicsBody& body = m_bodies[bodyId];
	body.setMotionType( type );
}

//
//Spatial queries

void physicsWorld::queryPoint( const Vector4& point, HitResult& hitResult ) const
{
	hitResult.numHits = 0;
	hitResult.hitInfos.clear();

	for ( auto i = 0; i < m_activeBodyIds.size(); i++ )
	{
		const physicsBody& body = getBody( m_activeBodyIds[i] );

		Vector4 pointLocal;
		pointLocal.setSub( point, body.getPosition() );
		pointLocal.setRotatedDir( -body.getRotation() );
		
		if ( body.getShape()->containsPoint( pointLocal ) )
		{
			hitResult.numHits++;

			HitResult::HitInfo hitInfo;
			{
				hitInfo.bodyId = m_activeBodyIds[i];
				hitInfo.hitPos.setSub( pointLocal, body.getPosition() );
			}
			hitResult.hitInfos.push_back( hitInfo );
		}
	}
}