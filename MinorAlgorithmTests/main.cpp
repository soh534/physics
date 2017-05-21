#include <vector>
#include <iostream>

/// Classifies contents of a and b into intersection and relative complements
/// i.e. c = ab', d = ab, e = a'b
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

template <typename T>
void printVector( const std::vector<T>& v )
{
	for (auto vi = v.begin(); vi != v.end(); vi++)
	{
		if (vi != v.begin())
		{
			std::cout << ", ";
		}
		std::cout << *vi;
	}
	std::cout << std::endl;
}

void classifySetsTest()
{
	std::vector<int> a, b, c, d, e;

	for ( int i = 0; i < 100; i++ )
	{
		if ( i % 3 == 0 )
		{
			a.push_back( i );
		}
		if ( i % 5 == 0 )
		{
			b.push_back( i );
		}
	}

	classifySets( a, b, c, d, e );

	printVector( c );
	printVector( d );
	printVector( e );
}

#include <physicsTypes.h>
#include <algorithm>

void bodyIdPairSortTest()
{
	std::vector<unsigned int> v;
	for ( int i = 0; i < 10; i++ )
	{
		v.push_back( i );
	}

	std::vector<BodyIdPair> pairs;

	for ( int i = v.size() - 1; i >= 0; i-- )
	{
		for ( int j = i - 1; j >= 0; j-- )
		{
			BodyIdPair pair( i, j );
			Assert(pair.bodyIdA > pair.bodyIdB, "idA must be > idB");
			pairs.push_back(pair);
		}
	}

	//printVector(pairs);

	std::sort(pairs.begin(), pairs.end(), bodyIdPairLess);
}

int main( int argc, char* argv[] )
{
	classifySetsTest();

	bodyIdPairSortTest();

	__debugbreak();

	return 0;
}