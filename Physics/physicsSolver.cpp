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

void solveConstraintPairs( const SolverInfo& info, 
						   std::vector<ConstrainedPair>& solvePairs,
						   std::vector<SolverBody>& updatedBodiesOut )
{
	for ( int i = 0; i < (int)solvePairs.size(); i++ )
	{
		ConstrainedPair& pair = solvePairs[ i ];

		SolverBody& bodyA = updatedBodiesOut[ pair.bodyIdA ];
		SolverBody& bodyB = updatedBodiesOut[ pair.bodyIdB ];
		std::vector<Constraint>& constraints = pair.constraints;

		int numConstraints = (int)constraints.size();
		for ( int j = 0; j < numConstraints; j++ )
		{
			Constraint& constraint = constraints[ j ];

			Vector3 rA_world = constraint.rA.getRotatedDir( bodyA.ori );
			Vector3 rB_world = constraint.rB.getRotatedDir( bodyB.ori );

			Real error = constraint.error;

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

			Real imp = -1.f * ( Jv - error / info.m_deltaTime ) / JmJ;
			
#if 0
			if ( contact )
			{ // Accumulate impulse method for contact constraints
				Real newImpulse = std::max( constraint.accumImp + imp, 0.f );
				imp = newImpulse - constraint.accumImp;
				constraint.accumImp = newImpulse;
				//drawText(std::to_string(constraint.accumImp), bodyA.pos + rA_world);
			}
#endif
			/// Contact point arms
			//drawArrow(bodyA.pos, rA_world, RED);
			//drawArrow(bodyB.pos, rB_world, BLUE);

			/// Impulse applied @ contact point
			drawArrow( bodyA.pos + rA_world, jac.vA * imp, RED );
			drawArrow( bodyB.pos + rB_world, jac.vB * imp, BLUE );
			drawArrow( bodyA.pos, rA_world, RED );
			drawArrow( bodyB.pos, rB_world, BLUE );

			bodyA.v += jac.vA * imp * bodyA.mInv;
			bodyB.v += jac.vB * imp * bodyB.mInv;
			bodyA.w += jac.wA * imp * bodyA.iInv;
			bodyB.w += jac.wB * imp * bodyB.iInv;
		}
	}
}

void physicsSolver::solveConstraints( const SolverInfo& info,
									  std::vector<ConstrainedPair>& constraints,
									  std::vector<SolverBody>& solverBodies,
									  std::vector<physicsBody>& physicsBodies )
{
	/// Solve constraints, put satisfying velocities in solver bodies
	for ( int i = 0; i < info.m_numIter; i++ )
	{
		solveConstraintPairs( info, constraints, solverBodies );
	}

	/// Update flagged bodies
	for ( int activeBodyIdsIdx = 0; activeBodyIdsIdx < (int)solverBodies.size(); activeBodyIdsIdx++ )
	{
		const SolverBody& solverBody = solverBodies[activeBodyIdsIdx];
		physicsBody& body = physicsBodies[activeBodyIdsIdx];


		body.setLinearVelocity( solverBody.v );
		body.setAngularSpeed( solverBody.w( 2 ) );
	}
}