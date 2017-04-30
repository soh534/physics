#include <physicsTypes.hpp>

BodyIdPair::BodyIdPair( const BodyId a, const BodyId b )
{
	set( a, b );
}

BodyIdPair::BodyIdPair( BodyIdPair const * other )
{
	set( other );
}

void BodyIdPair::set( const BodyId a, const BodyId b )
{
	bodyIdA = ( a > b ) ? a : b;
	bodyIdB = ( a > b ) ? b : a;
}

void BodyIdPair::set( BodyIdPair const * other )
{
	set( other->bodyIdA, other->bodyIdB );
}

bool operator == ( const BodyIdPair& pairA, const BodyIdPair& pairB )
{
	return ( pairA.bodyIdA == pairB.bodyIdA && pairA.bodyIdB == pairB.bodyIdB );
}

bool operator < ( const BodyIdPair& pairA, const BodyIdPair& pairB )
{
	return ( ( ( pairA.bodyIdA << 32 ) | pairA.bodyIdB ) < ( ( pairB.bodyIdA << 32 ) | pairB.bodyIdB ) );
}

bool operator > ( const BodyIdPair& pairA, const BodyIdPair& pairB )
{
	return !(pairA < pairB);
}

bool bodyIdPairLess( const BodyIdPair& pairA, const BodyIdPair& pairB )
{
	return pairA < pairB;
}

CollidedPair::CollidedPair( const BodyId a, const BodyId b )
	: BodyIdPair( a, b )
{

}

CollidedPair::CollidedPair(BodyIdPair const * other)
	: BodyIdPair( other )
{

}