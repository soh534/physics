#pragma once

#include <physicsTypes.h>

struct BodyIdPair
{
	BodyId bodyIdA;
	BodyId bodyIdB;

	BodyIdPair( const BodyId a = invalidId, const BodyId b = invalidId );
	BodyIdPair( const BodyIdPair& other );
	void set( const BodyId a, const BodyId b );
	void set( const BodyIdPair& other );
};

bool operator == ( const BodyIdPair& pairA, const BodyIdPair& pairB );
bool operator < ( const BodyIdPair& pairA, const BodyIdPair& pairB );
bool operator > ( const BodyIdPair& pairA, const BodyIdPair& pairB );
bool bodyIdPairLess( const BodyIdPair& pairA, const BodyIdPair& pairB );

namespace BodyIdPairsUtils
{
	/// Add contents of vector B to vector A, clear B after
	template <typename T>
	inline void movePairsBtoA( std::vector<T>& a, std::vector<T>& b )
	{
		a.insert( std::end( a ), std::begin( b ), std::end( b ) );
		b.clear();
	}

	/// Classifies contents of a and b into intersection and relative complements
	/// i.e. c = ab', d = ab, e = a'b
	/// Contents of a and b must be sorted in ascending order
	template <typename T1, typename T2>
	void classifyPairSets( const std::vector<T1>& a, const std::vector<T2>& b,
						   std::vector<T2>& c, std::vector<T2>& d, std::vector<T2>& e )
	{
		auto iterA = a.begin();
		auto iterB = b.begin();

		while ( iterA != a.end() || iterB != b.end() )
		{
			if ( iterA == a.end() )
			{
				e.insert( std::end( e ), iterB, std::end( b ) );
				break;
			}
			if ( iterB == b.end() )
			{
				c.insert( std::end( c ), iterA, std::end( a ) );
				break;
			}

			if ( *iterA == *iterB )
			{
				d.push_back( *iterB );
				iterA++;
				iterB++;
			}
			else if ( *iterA < *iterB )
			{
				c.push_back( *iterA );
				iterA++;
			}
			else if ( *iterA > *iterB )
			{
				e.push_back( *iterB );
				iterB++;
			}
		}
	}

	/// TODO: Find other way to do this as this is costly
	template <typename T1, typename T2>
	void deletePairsBfromA( std::vector<T1>& a, const std::vector<T2>& b )
	{
		auto iterA = a.begin();
		auto iterB = b.begin();

		while ( iterA != a.end() || iterB != b.end() )
		{
			if ( iterA == a.end() )
			{
				break;
			}
			if ( iterB == b.end() )
			{
				break;
			}

			if ( *iterA == *iterB )
			{
				iterA = a.erase( iterA );
			}
			else if ( *iterA < *iterB )
			{
				iterA++;
			}
			else if ( *iterA > *iterB )
			{
				iterB++;
			}
		}
	}
};