#pragma once

#include <vector>
#include <Base.h>

#include <physicsBody.h> // For physicsMotionType
#include <physicsShape.h> // For physicsShape::NUM_SHAPES
#include <physicsCollider.h>
#include <physicsSolver.h>

struct ContactPoint;
class physicsSolver;

// Separate a lot of these typedefs, internally used structs to internal types header
typedef void( *ColliderFuncPtr )( const physicsShape* shapeA, 
								  const physicsShape* shapeB,
								  const Transform& transformA,
								  const Transform& transformB,
								  std::vector<ContactPoint>& contacts);

struct physicsWorldConfig
{
	Vector4 m_gravity;
	Real m_deltaTime;
	Real m_cor;
	int m_numIter;

	physicsWorldConfig() :
		m_gravity( 0.f, -98.1f ),
		m_deltaTime( .016f ),
		m_cor( 1.f ),
		m_numIter( 8 ) {}
};

struct JointConfig
{
	int bodyIdA;
	int bodyIdB;
	Vector4 pivot;
};

struct CachedPair : public BodyIdPair
{
	ContactPoint cpA;
	ContactPoint cpB;
	Real accumImp;
	int numContacts;
	int idx;

public:

	CachedPair( const BodyId a, const BodyId b ):
		BodyIdPair( a, b ),
		cpA(), cpB(), accumImp( 0.f ), numContacts( 0 ), idx( 0 )
	{

	}

	CachedPair( const BodyIdPair& other ) :
		BodyIdPair( other ),
		cpA(), cpB(), accumImp( 0.f ), numContacts( 0 ), idx( 0 )
	{

	}

	// TODO: move implementation out
	void addContact(const ContactPoint& cp)
	{
/*
		if ( idx == 0 )
		{
			cpA = cp;
			numContacts = 1;
		}
		if ( idx == 1 )
		{
			cpB = cp;

			if ( numContacts < 2 )
			{
				numContacts = 2;
			}
		}
		idx = ( idx + 1 ) % 2;
*/
		if ( numContacts == 0 )
		{
			cpA = cp;
			numContacts = 1;
		}
		else if ( numContacts == 1 )
		{
			Real distToA;
			ContactPointUtils::getContactDifference( cp, cpA, distToA );
			if ( distToA < 5.f )
			{
				cpA = cp;
				numContacts = 1;
			}
			else
			{
				cpB = cp;
				numContacts = 2;
			}
		}
		else
		{
			Real distToA, distToB;
			ContactPointUtils::getContactDifference( cp, cpA, distToA );
			ContactPointUtils::getContactDifference( cp, cpB, distToB );

			if ( distToA < 5.f )
			{
				cpA = cp;
			}
			else if ( distToB < 5.f )
			{
				cpB = cp;
			}
			else
			{
				cpA = cp;
				numContacts = 1;
			}
		}
	}
/*	
	ContactPoint getContact()
	{
		if ( idx == 0 )
		{
			return cpA;
		}
		if ( idx == 1 )
		{
			return cpB;
		}
	}
*/
};

struct BroadphaseBody
{
	BodyId bodyId;
	physicsAabb aabb;

	BroadphaseBody( const BodyId bodyId, const physicsAabb& aabb ) :
		bodyId( bodyId ),
		aabb( aabb )
	{

	}
};

struct HitResult
{
	struct HitInfo
	{
		BodyId bodyId;  // BodyId of hit body
		Vector4 hitPos; // Hit position local to body
	};

	int numHits;
	std::vector<HitInfo> hitInfos;
};

class physicsWorld : public physicsObject
{
public:

	physicsWorld( const physicsWorldConfig& cinfo );

	~physicsWorld();

	BodyId createBody( const physicsBodyCinfo& cinfo );

	void removeBody( const BodyId bodyId );

	const std::vector<BodyId>& getActiveBodyIds() const { return m_activeBodyIds; }

	// TODO: add O(1) check which checks body is active
	const physicsBody& getBody( const BodyId bodyId ) const { return m_bodies[bodyId]; }

	int addJoint( const JointConfig& config );

	void removeJoint( JointId jointId );

	void step();
	
	// Utility funcs
	void setPosition( BodyId bodyId, const Vector4& point );

	physicsMotionType getMotionType( BodyId bodyId ) const;
	void setMotionType( BodyId bodyId, physicsMotionType type );

	const Real getDeltaTime() const { return m_solverInfo.m_deltaTime; }

	const std::vector<BroadphaseBody>& getBroadphaseBodies() const { return m_broadphaseBodies; }

	// Spatial query
    // Return first body which occupies point
	void queryPoint( const Vector4& point, HitResult& hitResult ) const;

protected:

	Vector4 m_gravity;
	Real m_cor;

	// Array of bodies, both simulated and freed
	std::vector<physicsBody> m_bodies;

    // Array of aabb's used for last step's broadphase
	std::vector<struct BroadphaseBody> m_broadphaseBodies;
	SolverInfo m_solverInfo;
	physicsSolver* m_solver;
	ColliderFuncPtr m_dispatchTable[physicsShape::NUM_SHAPES][physicsShape::NUM_SHAPES];

	// New broadphase pairs
	std::vector<BodyIdPair> m_newPairs;

	// Existing broadphase pairs from last frame
	std::vector<BodyIdPair> m_existingPairs;

	std::vector<CachedPair> m_cachedPairs;
	std::vector<ConstrainedPair> m_jointSolvePairs;
	std::vector<ConstrainedPair> m_contactSolvePairs;

	// Array of body Ids for which body is simulated
	std::vector<BodyId> m_activeBodyIds;

	std::vector<SolverBody> m_solverBodies;
	BodyId m_firstFreeBodyId;
};
