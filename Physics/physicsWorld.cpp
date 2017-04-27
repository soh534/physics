#include <DebugUtils.hpp>

#include <Base.hpp>
#include <physicsObject.hpp>
#include <physicsTypes.hpp>
#include <physicsWorld.hpp>
#include <physicsBody.hpp>
#include <physicsCollider.hpp>
#include <physicsSolver.hpp>

#include <vector>


bool bothStatic( physicsBody* bodyA, physicsBody* bodyB )
{
	return (
		bodyA->getMotionType() == physicsMotionType::STATIC &&
		bodyB->getMotionType() == physicsMotionType::STATIC );
}

physicsWorld::physicsWorld( const physicsWorldCinfo& cinfo ) :
	m_gravity( cinfo.m_gravity ),
	m_cor( cinfo.m_cor )
{
	m_solver = new physicsSolver( cinfo.m_deltaTime, cinfo.m_numIter );

	registerColliderCreateFunc( physicsShape::BASE, physicsShape::BASE, nullptr );
	registerColliderCreateFunc( physicsShape::BASE, physicsShape::CIRCLE, nullptr );
	registerColliderCreateFunc( physicsShape::BASE, physicsShape::BOX, nullptr );
	registerColliderCreateFunc( physicsShape::BASE, physicsShape::CONVEX, nullptr );
	registerColliderCreateFunc( physicsShape::CIRCLE, physicsShape::CIRCLE, physicsCircleCollider::create );
	registerColliderCreateFunc( physicsShape::CIRCLE, physicsShape::BOX, physicsCircleBoxCollider::create );
	registerColliderCreateFunc( physicsShape::CIRCLE, physicsShape::CONVEX, physicsConvexCollider::create );
	registerColliderCreateFunc( physicsShape::BOX, physicsShape::BOX, physicsBoxCollider::create );
	registerColliderCreateFunc( physicsShape::BOX, physicsShape::CONVEX, physicsConvexCollider::create );
	registerColliderCreateFunc( physicsShape::CONVEX, physicsShape::CONVEX, physicsConvexCollider::create );
}

physicsWorld::~physicsWorld()
{
	delete m_solver;
	m_bodies.clear();
}

void physicsWorld::registerColliderCreateFunc( physicsShape::Type typeA, physicsShape::Type typeB, CreateColliderFunc func )
{
	m_dispatchTable[ typeA ][ typeB ] = func;
	m_dispatchTable[ typeB ][ typeA ] = func;
}

static void sortBodyByType( physicsBody const *& bodyA, physicsBody const *& bodyB )
{
	Assert( bodyA != bodyB, "Trying to sort pointers to same bodies" );

	physicsShape::Type typeA = bodyA->getShapeType();
	physicsShape::Type typeB = bodyB->getShapeType();

	if ( typeA > typeB )
	{
		std::swap( bodyA, bodyB );
	}
}

physicsCollider* physicsWorld::createCollider( physicsBody const * bodyA, physicsBody const * bodyB )
{
	sortBodyByType( bodyA, bodyB );
	physicsShape::Type typeA = bodyA->getShapeType();
	physicsShape::Type typeB = bodyB->getShapeType();
	CreateColliderFunc func = m_dispatchTable[ typeA ][ typeB ];
	return func();
}

BodyId physicsWorld::addBody( physicsBody* const body )
{
	m_bodies.push_back( body );

	BodyId solverBodyId = m_solver->addSolverBody( body );

	body->setBodyId( (int)m_bodies.size() - 1 );

	Assert( solverBodyId == body->getBodyId(), "mismatching body IDs" );

	return body->getBodyId();
}

int physicsWorld::addJoint( const JointConfig& config )
{
	ConstrainedPair joint;
	{
		joint.bIdA = config.bodyIdA;
		joint.bIdB = config.bodyIdB;

		physicsBody* bodyA = m_bodies[ joint.bIdA ];
		physicsBody* bodyB = m_bodies[ joint.bIdB ];

		Vector3 rAworld = config.pivot - bodyA->getPosition();
		Vector3 rBworld = config.pivot - bodyB->getPosition();

		Vector3 rAlocal = rAworld.getRotatedDir( -bodyA->getRotation() );
		Vector3 rBlocal = rBworld.getRotatedDir( -bodyB->getRotation() );

		{ // Constraint x
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

		{ // Constraint y
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

	return m_solver->addJointConstraint( joint );
}

void physicsWorld::removeBody( BodyId bodyId )
{
	physicsBody* body = getBody( bodyId );
	delete body;
	m_bodies[ bodyId ] = nullptr;
}

void physicsWorld::removeJoint( JointId jointId )
{
	m_solver->removeJointConstraint( jointId );

}

physicsBody const * const physicsWorld::getBody( BodyId bodyId ) const
{
	return m_bodies[ bodyId ];
}

physicsBody* physicsWorld::getBody( BodyId bodyId )
{
	return m_bodies[ bodyId ];
}

void physicsWorld::step()
{
	std::vector<CollidedPair> existingCollisions;
	std::vector<CollidedPair> newCollisions;
	std::vector<BodyIdPair> lostCollisions;
	stepCollide( existingCollisions, newCollisions, lostCollisions );
	stepSolve( existingCollisions, newCollisions, lostCollisions );
}

void physicsWorld::stepSolve(
	const std::vector<CollidedPair>& existingCollisionsIn,
	const std::vector<CollidedPair>& newCollisionsIn,
	const std::vector<BodyIdPair>& lostCollisionsIn )
{
	/// Apply gravity, then solve constraints
	int numBodies = (int)m_bodies.size();

	for ( int i = 0; i < numBodies; i++ )
	{
		physicsBody* body = m_bodies[ i ];

		if ( !body->isStatic() )
		{
			Vector3& v = body->getLinearVelocity();
			v += m_gravity * m_solver->getDeltaTime();
		}
	}

	m_solver->preSolve( existingCollisionsIn, newCollisionsIn, lostCollisionsIn, m_bodies );
	m_solver->solveConstraints( m_bodies );

	/// Time integration
	for ( int i = 0; i < numBodies; i++ )
	{
		physicsBody* body = m_bodies[ i ];

		if ( !body->isStatic() )
		{
			const Vector3& v = body->getLinearVelocity();
			Vector3& p = body->getPosition();
			p += v * m_solver->getDeltaTime();

			const Real& w = body->getAngularSpeed();
			Real& rot = body->getRotation();
			rot += w * m_solver->getDeltaTime();
		}
	}
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

void physicsWorld::broadPhase( std::vector<BodyIdPair>& broadPhasePassedPairsOut )
{
	// Update AABB's, do 1D sweep & prune, add pairs which have overlapping AABB's
	std::vector<aabbIndex> aabbIndices;

	for ( int i = 0; i < (int)m_bodies.size(); i++ )
	{
		physicsBody * const body = m_bodies[ i ];

		body->updateAabb();

		DebugUtils::drawBpAabb( body );
	}

	for ( int i = 0; i < (int)m_bodies.size(); i++ )
	{
		physicsBody const * const body = m_bodies[ i ];

		physicsAabb aabb = body->getAabb();
		BodyId bId = body->getBodyId();
		aabbIndices.push_back( aabbIndex( bId, aabb.m_min ) );
		aabbIndices.push_back( aabbIndex( bId, aabb.m_max ) );
	}

	std::sort( aabbIndices.begin(), aabbIndices.end(), xless );

	std::vector<bool> flags( m_bodies.size(), false );

	for ( int i = 0; i < (int)aabbIndices.size(); i++ )
	{
		if ( aabbIndices[ i ].m_idx == -1 ) continue;

		std::fill( flags.begin(), flags.end(), false );

		for ( int j = i + 1; j < (int)aabbIndices.size(); j++ )
		{
			if ( aabbIndices[ j ].m_idx == -1 ) continue;

			if ( aabbIndices[ i ].m_idx == aabbIndices[ j ].m_idx )
			{
				aabbIndices[ i ].m_idx = -1;
				aabbIndices[ j ].m_idx = -1;
				break;
			}

			if ( flags[ aabbIndices[ j ].m_idx ] ) continue;

			physicsBody* bodyA = m_bodies[ aabbIndices[ i ].m_idx ];
			physicsBody* bodyB = m_bodies[ aabbIndices[ j ].m_idx ];

			if ( bodyA->isStatic() && bodyB->isStatic() ) continue;

			if ( !checkCollidable( bodyA->getBodyId(), bodyB->getBodyId() ) ) continue;

			physicsAabb aabbA = bodyA->getAabb();
			physicsAabb aabbB = bodyB->getAabb();

			if ( aabbA.overlaps( aabbB ) )
			{
				BodyIdPair pair;
				{
					pair.bIdA = bodyA->getBodyId();
					pair.bIdB = bodyB->getBodyId();
				}

				broadPhasePassedPairsOut.push_back( pair );

				flags[ bodyB->getBodyId() ] = true;
			}
		}
	}
}

void physicsWorld::narrowPhase( const std::vector<BodyIdPair>& broadPhasePassedPairs, std::vector<CollidedPair>& collisionsOut )
{
	for ( auto bodyIdPair = broadPhasePassedPairs.begin();
		  bodyIdPair != broadPhasePassedPairs.end();
		  bodyIdPair++ )
	{
		physicsBody const * bodyA = getBody( bodyIdPair->bIdA );
		physicsBody const * bodyB = getBody( bodyIdPair->bIdB );

		physicsCollider* collider = createCollider( bodyA, bodyB );

		CollidedPair collision;
		collider->collide( bodyA, bodyB, collision.cp );

		if ( collision.cp.size() > 0 )
		{
			collision.bIdA = bodyIdPair->bIdA;
			collision.bIdB = bodyIdPair->bIdB;
			collisionsOut.push_back( collision );
		}

		delete collider; /// TODO: don't delete, cache collision in collider maybe?
	}
}


namespace BodyIdPairsUtils
{
	/// Add contents of vector B to vector A, clear B after, implemented just to improve readability
	template <typename T>
	inline void moveVecBtoA( std::vector<T>& a, std::vector<T>& b )
	{
		a.insert( std::end( a ), std::begin( b ), std::end( b ) );
		b.clear();
	}

	/// Classifies contents of a and b into intersection and relative complements
	/// i.e. c = ab', d = ab, e = a'b
	/// Contents of a and b must be sorted in same order
	template<typename T>
	void classifySets(
		const std::vector<T>& a,
		const std::vector<T>& b,
		std::vector<T>& c,
		std::vector<T>& d,
		std::vector<T>& e )
	{
		auto iterA = a.begin();
		auto iterB = b.begin();

		while (iterA != a.end() || iterB != b.end())
		{
			if (iterA == a.end())
			{
				e.insert( std::end( e ), iterB, std::end( b ) );
				break;
			}
			if (iterB == b.end())
			{
				c.insert( std::end( c ), iterA, std::end( a ) );
				break;
			}

			if (*iterA == *iterB)
			{
				d.push_back( *iterB );
				iterA++;
				iterB++;
			}
			else if (*iterA < *iterB)
			{
				c.push_back( *iterA );
				iterA++;
			}
			else if (*iterA > *iterB)
			{
				e.push_back( *iterB );
				iterB++;
			}
		}
	}
}

void physicsWorld::stepCollide(
	std::vector<CollidedPair>& sustainedCollisionsOut,
	std::vector<CollidedPair>& newCollisionsOut,
	std::vector<BodyIdPair>& lostCollisionsOut )
{
	/// 1. Add the pairs found in last frame to existing pairs
	/// 2. Look through existing pairs and pairs within this frame to classify new & existing pairs

	/// Broadphase
	std::vector<BodyIdPair> bpPassedPairs;
	broadPhase( bpPassedPairs );
	std::sort( bpPassedPairs.begin(), bpPassedPairs.end(), bodyIdPairLess );

	BodyIdPairsUtils::moveVecBtoA( m_existingPairs, m_newPairs );

	std::vector<BodyIdPair> bpRemainedPairs, bpLostPairs;
	BodyIdPairsUtils::classifySets( m_existingPairs, 
									bpPassedPairs, 
									lostCollisionsOut, 
									bpRemainedPairs,
									m_newPairs );

	/// Narrowphase
	std::vector<CollidedPair> npPassedPairs;
	narrowPhase( bpPassedPairs, npPassedPairs );

	BodyIdPairsUtils::moveVecBtoA( m_existingCollidedPairs, m_newCollidedPairs );

	std::vector<CollidedPair> npRemainedPairs, npLostPairs;
	BodyIdPairsUtils::classifySets( m_existingCollidedPairs, 
									npPassedPairs, 
									npLostPairs,
									npRemainedPairs,
									m_newCollidedPairs );

	/// TODO: replace this with O(N)
	for ( int i = 0; i < (int)bpRemainedPairs.size(); i++ )
	{
		for ( int j = 0; j < (int)npRemainedPairs.size(); j++ )
		{
			if ( bpRemainedPairs[ i ] == npRemainedPairs[ j ] )
			{
				sustainedCollisionsOut.push_back( npRemainedPairs[ j ] );
			}
		}
	}

	m_existingCollidedPairs = sustainedCollisionsOut;
	newCollisionsOut = m_newCollidedPairs;
}

bool physicsWorld::checkCollidable( BodyId bodyIdA, BodyId bodyIdB )
{
	physicsBody const * bodyA = getBody( bodyIdA );
	physicsBody const * bodyB = getBody( bodyIdB );

	if ( bodyA->getBodyFilter() == bodyB->getBodyFilter() )
	{
		return true;
	}

	return false;
}

/// TODO: move this function outside of physics
void physicsWorld::render()
{
	for (auto body = m_bodies.begin(); body != m_bodies.end(); body++)
	{
		(*body)->render();
	}
}

void physicsWorld::setPosition( BodyId bodyId, const Vector3& point )
{
	physicsBody* body = getBody( bodyId );
	body->setPosition( point );
}

void physicsWorld::setMotionType( BodyId bodyId, physicsMotionType type )
{
	physicsBody* body = getBody( bodyId );
	body->setMotionType( type );
}

const Real physicsWorld::getDeltaTime()
{
	return m_solver->getDeltaTime();
}
