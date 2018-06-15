#pragma once

#include <physicsInternalTypes.h>
#include <physicsTypes.h>
#include <algorithm>
#include <vector>

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
            Assert( pair.bodyIdA > pair.bodyIdB, "idA must be > idB" );
            pairs.push_back( pair );
        }
    }

    //printVector(pairs);

    std::sort( pairs.begin(), pairs.end(), bodyIdPairLess );
}