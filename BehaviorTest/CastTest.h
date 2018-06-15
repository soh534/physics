#pragma once

struct Strukt
{
    int a, b, c, d;

    Strukt()
        : a( 0 ), b( 1 ), c( 2 ), d( 3 )
    {
    
    }
};

struct BiggerStrukt
{
    int a, b, c, d, e;

    BiggerStrukt()
        : a( 0 ), b( 1 ), c( 2 ), d( 3 ), e( 4 )
    {
    
    }
};

void castTest()
{
    Strukt s[2];

    int* e = reinterpret_cast<int*>(s);
    *e = 10;

    int* f = reinterpret_cast<int*>(s);

    BiggerStrukt* g = reinterpret_cast<BiggerStrukt*>( s );

    g->a = 5;
    g->b = 6;
    g->c = 7;
    g->d = 8;
    g->e = 9; // This re-writes a for s[1] for this particular case

    return;
}
