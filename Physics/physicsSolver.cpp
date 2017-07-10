#include <algorithm>

#include <Base.h>
#include <physicsTypes.h>
#include <physicsBody.h>
#include <physicsSolver.h>

#include <DebugUtils.h>

/*
{ // Add friction
	Constraint friction;
	friction.rA = contactPoint.getContactA();
	friction.rB = contactPoint.getContactB();
	friction.accumImp = 0.f;
	friction.error = 0.f;

	const Vector3& n = contactPoint.getNormal().getRotatedDir( 90.f * g_degToRad );
	const SolverBody& bodyA = getSolverBody( constrainedPair.bIdA );
	const SolverBody& bodyB = getSolverBody( constrainedPair.bIdB );
	const Vector3& rA_world = friction.rA.getRotatedDir( bodyA.ori );
	const Vector3& rB_world = friction.rB.getRotatedDir( bodyB.ori );

	friction.jac.vA = n;
	friction.jac.vB = n.getNegated();
	friction.jac.wA = rA_world.cross( n );
	friction.jac.wB = rB_world.cross( n.getNegated() );

	constrainedPair.constraints.push_back( friction );
}
*/

void SolverBody::setFromBody( const physicsBody& body )
{
	v = body.getLinearVelocity();
	w( 2 ) = body.getAngularSpeed();
	pos = body.getPosition();
	ori = body.getRotation();
	mInv = body.getInvMass();
	iInv = body.getInvInertia();
}

void solveConstrainedPairs( const SolverInfo& info, 
							bool isContact,
							std::vector<ConstrainedPair>& solvePairs,
							std::vector<SolverBody>& updatedBodiesOut )
{
	for ( auto pairIdx = 0; pairIdx < solvePairs.size(); pairIdx++ )
	{
		ConstrainedPair& pair = solvePairs[ pairIdx ];

		SolverBody& bodyA = updatedBodiesOut[ pair.bodyIdA ];
		SolverBody& bodyB = updatedBodiesOut[ pair.bodyIdB ];
		std::vector<Constraint>& constraints = pair.constraints;

		for ( auto constraintIdx = 0; constraintIdx < constraints.size(); constraintIdx++ )
		{
			Constraint& constraint = constraints[ constraintIdx ];

			Vector3 rA_world = constraint.rA.getRotatedDir( bodyA.ori ); /// TODO: replace with transform
			Vector3 rB_world = constraint.rB.getRotatedDir( bodyB.ori );

			const Jacobian& jac = constraint.jac;

			Vector3 vab = bodyA.v + bodyA.w.cross( rA_world ) - bodyB.v - bodyB.w.cross( rB_world );

			Real Jv = vab.dot(jac.vA);

			Real JmJ =
				jac.vA( 0 ) * bodyA.mInv * jac.vA( 0 ) +
				jac.vA( 1 ) * bodyA.mInv * jac.vA( 1 ) +
				jac.wA( 2 ) * bodyA.iInv * jac.wA( 2 ) +
				jac.vB( 0 ) * bodyB.mInv * jac.vB( 0 ) +
				jac.vB( 1 ) * bodyB.mInv * jac.vB( 1 ) +
				jac.wB( 2 ) * bodyB.iInv * jac.wB( 2 );

			Real bias = isContact ? 1.0f : 1.f;

			Real impulse = -1.f * ( Jv - bias*( constraint.error / info.m_deltaTime ) ) / JmJ;

			Assert( !isinf( impulse ), "infinite impulse in solver" );
			Assert( !isnan( impulse ), "nan impulse in solver" );

			if ( isContact )
			{ 
				Real newImpulse = std::max( pair.accumImp + impulse, 0.f );
				impulse = newImpulse - pair.accumImp;
				pair.accumImp = newImpulse;
			}

			/// Impulse applied @ contact point
//			if ( isContact )
			if ( false )
			{
				drawArrow( bodyA.pos + rA_world, jac.vA * impulse * 0.01f, RED );
				drawArrow( bodyB.pos + rB_world, jac.vB * impulse * 0.01f, BLUE );
				drawArrow( bodyA.pos, rA_world, RED );
				drawArrow( bodyB.pos, rB_world, BLUE );
			}

			bodyA.v += jac.vA * impulse * bodyA.mInv;
			bodyB.v += jac.vB * impulse * bodyB.mInv;
			bodyA.w += jac.wA * impulse * bodyA.iInv;
			bodyB.w += jac.wB * impulse * bodyB.iInv;

			/// TODO: clean-up these sanity checks
			Assert( !bodyA.v.isInf(), "bodyA has infinite linear velocity in solver" );
			Assert( !bodyB.v.isInf(), "bodyB has infinite linear velocity in solver" );
			Assert( !bodyA.v.isNan(), "bodyA has nan linear velocity in solver" );
			Assert( !bodyB.v.isNan(), "bodyB has nan linear velocity in solver" );
			Assert( !bodyA.w.isInf(), "bodyA has infinite angular velocity in solver" );
			Assert( !bodyB.w.isInf(), "bodyB has infinite angular velocity in solver" );
			Assert( !bodyA.w.isNan(), "bodyA has nan angular velocity in solver" );
			Assert( !bodyB.w.isNan(), "bodyB has nan angular velocity in solver" );
		}
	}
}

void physicsSolver::solveConstraints( const SolverInfo& info,
									  bool isContact,
									  std::vector<ConstrainedPair>& constrainedPairs,
									  std::vector<SolverBody>& solverBodies,
									  std::vector<physicsBody>& physicsBodies )
{
	/// Solve constraints, put satisfying velocities in solver bodies
	for ( int i = 0; i < info.m_numIter; i++ )
	{
		solveConstrainedPairs( info, isContact, constrainedPairs, solverBodies );
	}

	/// Update body velocities
	for ( int activeBodyIdsIdx = 0; activeBodyIdsIdx < (int)solverBodies.size(); activeBodyIdsIdx++ )
	{
		const SolverBody& solverBody = solverBodies[activeBodyIdsIdx];
		physicsBody& body = physicsBodies[activeBodyIdsIdx];

		body.setLinearVelocity( solverBody.v );
		body.setAngularSpeed( solverBody.w( 2 ) );
	}
}