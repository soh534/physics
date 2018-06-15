#pragma once

struct Object
{
    float x, y, z;
    char c;
    double d;
};

bool structAlignmentTest()
{
    const int numElements = 1000;

    Object vectors[numElements];

    int szGuessed = numElements * (3 * sizeof( float ) + sizeof( char ) + sizeof( double ));
    int szActual = sizeof( vectors );

    szGuessed = 3 * sizeof( float ) + sizeof( char ) + sizeof( double );
    szActual = sizeof( Object );

    // Takeaway
    // How members of a struct are padded is not standardized, so no assumptions
    // Even if two structs with exact members but different order will cause different padding
    // Can enforce things via pragma pack though
    // https://docs.microsoft.com/en-us/previous-versions/visualstudio/visual-studio-2010/2e70t5y1(v=vs.100)

    return (szGuessed == szActual);
}