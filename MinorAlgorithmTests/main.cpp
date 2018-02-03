#include <vector>
#include <iostream>

#include <physicsInternalTypes.h>

// Classifies contents of a and b into intersection and relative complements
// i.e. c = ab', d = ab, e = a'b
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

#include <Transform.h>

void transformsTest()
{
	Vector4 v( 50.f, 50.f );
	Vector4 vopp = v.getNegated();

	Real rot( 45.f * g_degToRad );
	Real rotopp = -1.f * rot;

	Transform t( v, rot );
	Transform tinv; tinv.setInverse( t );
	v = tinv.getTranslation();
	rot = tinv.getRotation();
	t.setInverse( tinv );
	v = t.getTranslation();
	rot = t.getRotation();
	Transform id; id.setMul(t, tinv);

	Transform lin; lin.setTranslation( v );
	Transform ang; ang.setRotation( rot );
	Transform linAng; linAng.setMul( lin, ang ); // Remember v' = LRv, rotation applied first

	Transform linInv; linInv.setInverse( lin );
	Transform angInv; angInv.setInverse( ang );

	Transform angInvLinInv( vopp, rotopp );

	// v3(-10.f, 10.f)  ********* v0(10.f, 10.f)
	//                  *       *
	//                  *   0   * -> rotate 45 deg, then translate 50, 50
	//                  *       *
	// v2(-10.f, -10.f) ********* v1(10.f, -10.f)

	Vector4 v0( 10.f, 10.f );
	Vector4 v1( 10.f, -10.f );
	Vector4 v2( -10.f, -10.f );
	Vector4 v3( -10.f, 10.f );

	v0.setTransformedPos( ang, v0 );
	v1.setTransformedPos( ang, v1 );
	v2.setTransformedPos( ang, v2 );
	v3.setTransformedPos( ang, v3 );

	v0.setTransformedPos( lin, v0 );
	v1.setTransformedPos( lin, v1 );
	v2.setTransformedPos( lin, v2 );
	v3.setTransformedPos( lin, v3 );

	v0.set( 10.f, 10.f );
	v1.set( 10.f, -10.f );
	v2.set( -10.f, -10.f );
	v3.set( -10.f, 10.f );

	v0.setTransformedPos( linAng, v0 );
	v1.setTransformedPos( linAng, v1 );
	v2.setTransformedPos( linAng, v2 );
	v3.setTransformedPos( linAng, v3 );

	for ( float angle = 0.f; angle < 360.f; angle += 1.f )
	{
		//float c = cos( angle * g_degToRad );
		//float ac = acos( c ) / g_degToRad;
		//Assert( fabs( ac - angle ) < 0.001f, "faults" ); // Fails from angle = 181.f

		//float s = sin( angle * g_degToRad );
		//float as = asin( s ) / g_degToRad;
		//Assert( fabs( as - angle ) < 0.001f, "faults" ); // Fails from angle = 91.f

		Transform trot; trot.setRotation( angle * g_degToRad );
		Transform trott; trott.setRotation( trot.getRotation() );
		Assert( trot.isApproximatelyEqual( trott, 0.001f ), "faults" );
	}
}

int main( int argc, char* argv[] )
{
	classifySetsTest();

	bodyIdPairSortTest();

	transformsTest();

	__debugbreak();

	return 0;
}