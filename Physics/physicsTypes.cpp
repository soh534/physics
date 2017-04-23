#include <physicsTypes.hpp>

bool operator == ( const BodyIdPair& pairA, const BodyIdPair& pairB )
{
	return ( pairA.bIdA == pairB.bIdA && pairA.bIdB == pairB.bIdB );
}

bool operator < ( const BodyIdPair& pairA, const BodyIdPair& pairB )
{
	return ( ( ( pairA.bIdA << 32 ) | pairA.bIdB ) < ( ( pairB.bIdA << 32 ) | pairB.bIdB ) );
}

bool operator > ( const BodyIdPair& pairA, const BodyIdPair& pairB )
{
	return !(pairA < pairB);
}

bool bodyIdPairLess( const BodyIdPair& pairA, const BodyIdPair& pairB )
{
	return pairA < pairB;
}