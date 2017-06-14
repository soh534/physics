#include <vector>
#include <physicsInternalTypes.h>

BodyIdPair::BodyIdPair( const BodyId a, const BodyId b )
{
	set( a, b );
}

BodyIdPair::BodyIdPair( const BodyIdPair& other )
{
	set( other );
}

void BodyIdPair::set( const BodyId a, const BodyId b )
{
	if ( a != invalidId && b != invalidId )
	{
		Assert( a != b, "BodyIdPair must consist of IDs for two different bodies" );
	}

	bodyIdA = ( a > b ) ? a : b;
	bodyIdB = ( a > b ) ? b : a;
}

void BodyIdPair::set( const BodyIdPair& other )
{
	set( other.bodyIdA, other.bodyIdB );
}

bool operator == ( const BodyIdPair& pairA, const BodyIdPair& pairB )
{
	return ( pairA.bodyIdA == pairB.bodyIdA && pairA.bodyIdB == pairB.bodyIdB );
}

bool operator != ( const BodyIdPair& pairA, const BodyIdPair& pairB )
{
	return !( pairA == pairB );
}

bool operator < ( const BodyIdPair& pairA, const BodyIdPair& pairB )
{
	//	return ( ( ( pairA.bodyIdA << 16 ) | pairA.bodyIdB ) < ( ( pairB.bodyIdA << 16 ) | pairB.bodyIdB ) );
	if ( pairA.bodyIdA < pairB.bodyIdA )
	{
		return true;
	}

	if ( pairA.bodyIdA == pairB.bodyIdA )
	{
		return ( pairA.bodyIdB < pairB.bodyIdB );
	}

	return false;
}

bool operator > ( const BodyIdPair& pairA, const BodyIdPair& pairB )
{
	return !( pairA < pairB );
}

bool bodyIdPairLess( const BodyIdPair& pairA, const BodyIdPair& pairB )
{
	return pairA < pairB;
}
