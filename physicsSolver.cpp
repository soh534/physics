#include <Base.hpp>
#include <physicsTypes.hpp>
#include <physicsBody.hpp>
#include <physicsSolver.hpp>

#include <algorithm>

#include <DebugUtils.hpp>

using namespace std;

physicsSolver::physicsSolver(const Real deltaTime, const int numIterations)
	: m_deltaTime(deltaTime), m_numIter(numIterations)
{

}

physicsSolver::~physicsSolver()
{

}

BodyId physicsSolver::addSolverBody(physicsBody const * const body)
{
	SolverBody solverBody;
	{
		solverBody.v = body->getLinearVelocity();
		solverBody.w(2) = body->getAngularSpeed();
		solverBody.pos = body->getPosition();
		solverBody.ori = body->getRotation();
		solverBody.mInv = body->getInvMass();
		solverBody.iInv = body->getInvInertia();
	}

	m_solverBodies.push_back(solverBody);

	return ((int)m_solverBodies.size() - 1);
}

SolverBody& physicsSolver::getSolverBody(const BodyId bodyId)
{
	return m_solverBodies[bodyId];
}

int physicsSolver::addJointConstraint(const ConstrainedPair& joint)
{
	m_jointConstraintPairs.push_back(joint);
	return (m_jointConstraintPairs.size() - 1);
}

void physicsSolver::removeJointConstraint(JointId jointId)
{
	ConstrainedPair& joint = m_jointConstraintPairs[jointId];
	joint.bIdA = invalidId;
	joint.bIdB = invalidId;
	joint.constraints.clear();
}

void physicsSolver::addNewContacts(const vector<CollidedPair>& collisionsIn)
{
	// Update velocities, arms for contact constraints
	for (int i = 0; i < (int)collisionsIn.size(); i++)
	{
		const CollidedPair& collidedPair = collisionsIn[i];
		const vector<ContactPoint>& contacts = collidedPair.cp;

		ConstrainedPair constrainedPair;

		constrainedPair.bIdA = collidedPair.bIdA;
		constrainedPair.bIdB = collidedPair.bIdB;

		for (int i = 0; i < (int)contacts.size(); i++)
		{
			const ContactPoint& contactPoint = contacts[i];

#if 0
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

				constrainedPair.constraints.push_back(friction);
			}
#endif
			{ // Add normal constraint
				Constraint normal;
				normal.rA = contactPoint.getContactA();
				normal.rB = contactPoint.getContactB();
				normal.accumImp = 0.f;
				normal.error = contactPoint.getDepth();

				const Vector3& norm = contactPoint.getNormal();
				const SolverBody& bodyA = getSolverBody( constrainedPair.bIdA );
				const SolverBody& bodyB = getSolverBody( constrainedPair.bIdB );
				const Vector3& rA_world = normal.rA.getRotatedDir( bodyA.ori );
				const Vector3& rB_world = normal.rB.getRotatedDir( bodyB.ori );

				normal.jac.vA = norm;
				normal.jac.vB = norm.getNegated();
				normal.jac.wA = rA_world.cross( norm );
				normal.jac.wB = rB_world.cross( norm.getNegated() );

				constrainedPair.constraints.push_back( normal );
			}

			m_contactConstraintPairs.push_back( constrainedPair );
		}
	}
}

void physicsSolver::updateContacts( const vector<CollidedPair>& remainingCollisionsIn )
{
	vector<CollidedPair>::const_iterator remainedCollision = remainingCollisionsIn.begin();
	vector<ConstrainedPair>::iterator lastFrameCollision = m_contactConstraintPairs.begin();

	vector<ConstrainedPair> updatedContactConstraintPairs;

	if (remainedCollision != remainingCollisionsIn.end())
	{
		Assert(
			lastFrameCollision != m_contactConstraintPairs.end(),
			"there is an inexistent remaining collision from last frame"
		);
	}

	for (int i = 0; i < (int)remainingCollisionsIn.size(); i++)
	{
		for (int j = 0; j < (int)m_contactConstraintPairs.size(); j++)
		{
			if (remainingCollisionsIn[i] == m_contactConstraintPairs[j])
			{
				Constraint& constraint = m_contactConstraintPairs[j].constraints[0];

				const ContactPoint& contactPoint = remainedCollision->cp[0];

				constraint.rA = contactPoint.getContactA();
				constraint.rB = contactPoint.getContactB();
				constraint.error = contactPoint.getDepth();

				const Vector3& norm = contactPoint.getNormal();
				const SolverBody& bodyA = getSolverBody(remainedCollision->bIdA);
				const SolverBody& bodyB = getSolverBody(remainedCollision->bIdB);
				const Vector3& rA_world = constraint.rA.getRotatedDir(bodyA.ori);
				const Vector3& rB_world = constraint.rB.getRotatedDir(bodyB.ori);

				constraint.jac.vA = norm;
				constraint.jac.vB = norm.getNegated();
				constraint.jac.wA = rA_world.cross(norm);
				constraint.jac.wB = rB_world.cross(norm.getNegated());
			}
		}
	}

/*
	while (
		remainedCollision != remainingCollisionsIn.end() ||
		lastFrameCollision != m_contactConstraintPairs.end())
	{
		if (remainedCollision == remainingCollisionsIn.end() &&
			lastFrameCollision == m_contactConstraintPairs.end())
		{
			if (lastFrameCollision->bIdA == remainedCollision->bIdA &&
				lastFrameCollision->bIdB == remainedCollision->bIdB)
			{
				Constraint& constraint = lastFrameCollision->constraints[0];

				const ContactPoint& contactPoint = remainedCollision->cp[0];

				constraint.rA = contactPoint.getContactA();
				constraint.rB = contactPoint.getContactB();
				constraint.error = contactPoint.getDepth();

				const Vector3& norm = contactPoint.getNormal();
				const SolverBody& bodyA = getSolverBody(remainedCollision->bIdA);
				const SolverBody& bodyB = getSolverBody(remainedCollision->bIdB);
				const Vector3& rA_world = constraint.rA.getRotatedDir(bodyA.ori);
				const Vector3& rB_world = constraint.rB.getRotatedDir(bodyB.ori);

				constraint.jac.vA = norm;
				constraint.jac.vB = norm.getNegated();
				constraint.jac.wA = rA_world.cross(norm);
				constraint.jac.wB = rB_world.cross(norm.getNegated());

				remainedCollision++;
				lastFrameCollision++;
			}
			else if (*lastFrameCollision < *remainedCollision)
			{
				lastFrameCollision->bIdA = invalidId;
				lastFrameCollision->bIdB = invalidId;
				lastFrameCollision++;
			}
		}
	}
*/
}

void physicsSolver::updateJointConstraints(const vector<physicsBody*>& bodies)
{
	for (int i = 0; i < (int)m_jointConstraintPairs.size(); i++)
	{
		ConstrainedPair& jointConstraint = m_jointConstraintPairs[i];

		if (jointConstraint.bIdA == invalidId && jointConstraint.bIdB == invalidId)
		{
			continue;
		}

		const SolverBody& bodyA = getSolverBody( jointConstraint.bIdA );
		const SolverBody& bodyB = getSolverBody( jointConstraint.bIdB );

		Constraint& constraintX = jointConstraint.constraints[0];
		const Vector3& rAworldx = constraintX.rA.getRotatedDir(bodyA.ori);
		const Vector3& rBworldx = constraintX.rB.getRotatedDir(bodyB.ori);

		constraintX.error = -(bodyA.pos + rAworldx - bodyB.pos - rBworldx)(0);
		constraintX.jac.wA = rAworldx.cross(constraintX.jac.vA);
		constraintX.jac.wB = rBworldx.cross(constraintX.jac.vB);

		Constraint& constraintY = jointConstraint.constraints[1];
		const Vector3& rAworldy = constraintY.rA.getRotatedDir(bodyA.ori);
		const Vector3& rBworldy = constraintY.rB.getRotatedDir(bodyB.ori);

		constraintY.error = -(bodyA.pos + rAworldy - bodyB.pos - rBworldy)(1);
		constraintY.jac.wA = rAworldy.cross(constraintY.jac.vA);
		constraintY.jac.wB = rBworldy.cross(constraintY.jac.vB);
	}
}

void physicsSolver::preSolve(
	const vector<CollidedPair>& remainingCollisionsIn,
	const vector<CollidedPair>& newCollisionsIn,
	const vector<BodyIdPair>& lostCollisionsIn,
	const vector<physicsBody*>& bodies)
{
	// Set solver body velocities, update constraints
	for (int i = 0; i < (int)bodies.size(); i++)
	{
		physicsBody const * const body = bodies[i];
		SolverBody& solverBody = getSolverBody(body->getBodyId());

		solverBody.v = body->getLinearVelocity();
		solverBody.w(2) = body->getAngularSpeed();
		solverBody.pos = body->getPosition();
		solverBody.ori = body->getRotation();
	}

	/// Remove constrained pairs that have lost broadphase
	vector<ConstrainedPair>::iterator iter = m_contactConstraintPairs.begin();
	while (iter != m_contactConstraintPairs.end())
	{
		bool erased = false;
		for (int j = 0; j < (int)lostCollisionsIn.size(); j++)
		{
			if (*iter == lostCollisionsIn[j])
			{
				iter = m_contactConstraintPairs.erase(iter);
				erased = true;
				break;
			}
		}
		if (!erased)
		{
			iter++;
		}
	}

	/// At this point m_contactConstraintPairs must be sorted
	updateContacts(remainingCollisionsIn);
	updateJointConstraints(bodies);
	addNewContacts(newCollisionsIn);
	
	/// Re-sort for next frame and for determinism
	std::sort( m_contactConstraintPairs.begin(), m_contactConstraintPairs.end(), bodyIdPairLess );
	std::sort( m_jointConstraintPairs.begin(), m_jointConstraintPairs.end(), bodyIdPairLess );
}

void physicsSolver::solveConstraints(vector<physicsBody*>& updatedBodiesOut)
{
	// Solve joint and contact constraints, update velocities of constrained bodies
	for (int i = 0; i < m_numIter; i++)
	{
		solveConstraintPairs(m_jointConstraintPairs, false);
		solveConstraintPairs(m_contactConstraintPairs, true);
	}

	vector<bool> flags(updatedBodiesOut.size(), false);

	// Flag bodies
	for (int i = 0; i < (int)m_jointConstraintPairs.size(); i++)
	{
		const ConstrainedPair& joint = m_jointConstraintPairs[i];
		flags[joint.bIdA] = 1;
		flags[joint.bIdB] = 1;
	}

	for (int i = 0; i < (int)m_contactConstraintPairs.size(); i++)
	{
		const ConstrainedPair& contact = m_contactConstraintPairs[i];
		flags[contact.bIdA] = 1;
		flags[contact.bIdB] = 1;
	}

	// Update flagged bodies
	for (int bodyId = 0; bodyId < (int)updatedBodiesOut.size(); bodyId++)
	{
		if (flags[bodyId])
		{
			physicsBody* body = updatedBodiesOut[bodyId];
			const SolverBody& solverBody = m_solverBodies[bodyId];

			body->setLinearVelocity(solverBody.v);
			body->setAngularSpeed(solverBody.w(2));
		}
	}

	//m_contactConstraintPairs.clear();
}

void physicsSolver::solveConstraintPairs(std::vector<ConstrainedPair>& pairsIn, bool contact)
{
	for (int i = 0; i < (int)pairsIn.size(); i++)
	{
		ConstrainedPair& constraint = pairsIn[i];

		SolverBody& bodyA = m_solverBodies[constraint.bIdA];
		SolverBody& bodyB = m_solverBodies[constraint.bIdB];
		std::vector<Constraint>& constraints = constraint.constraints;

		for (int j = 0; j < (int)constraints.size(); j++)
		{
			Constraint& constraint = constraints[j];
			Vector3 rA_world = constraint.rA.getRotatedDir( bodyA.ori );
			Vector3 rB_world = constraint.rB.getRotatedDir( bodyB.ori );

			Real error = constraint.error;

			const Jacobian& jac = constraint.jac;

			Vector3 vab = bodyA.v + bodyA.w.cross(rA_world) - bodyB.v - bodyB.w.cross(rB_world);

			Real Jv = vab.dot(jac.vA);

			Real JmJ =
				jac.vA(0) * bodyA.mInv * jac.vA(0) +
				jac.vA(1) * bodyA.mInv * jac.vA(1) +
				jac.wA(2) * bodyA.iInv * jac.wA(2) +
				jac.vB(0) * bodyB.mInv * jac.vB(0) +
				jac.vB(1) * bodyB.mInv * jac.vB(1) +
				jac.wB(2) * bodyB.iInv * jac.wB(2);

			Real imp = -1.f * (Jv - error / m_deltaTime) / JmJ;
			
#if 1
			if (contact)
			{ // Accumulate impulse method for contact constraints
				Real newImpulse = std::max(constraint.accumImp + imp, 0.f);
				imp = newImpulse - constraint.accumImp;
				constraint.accumImp = newImpulse;
				contact = contact;
			}
#endif
			/// Contact point arms
			//drawArrow(bodyA.pos, rA_world, RED);
			//drawArrow(bodyB.pos, rB_world, BLUE);

			/// Impulse applied @ contact point
			drawArrow(bodyA.pos + rA_world, jac.vA * imp, RED);
			drawArrow(bodyB.pos + rB_world, jac.vB * imp, BLUE);

			bodyA.v += jac.vA * imp * bodyA.mInv;
			bodyB.v += jac.vB * imp * bodyB.mInv;
			bodyA.w += jac.wA * imp * bodyA.iInv;
			bodyB.w += jac.wB * imp * bodyB.iInv;
		}
	}
}