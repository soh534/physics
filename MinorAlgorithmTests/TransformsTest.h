#pragma once

#include <Transform.h>

#include <physicsInternalTypes.h>

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
    Transform id; id.setMul( t, tinv );

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