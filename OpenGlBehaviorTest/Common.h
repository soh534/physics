#pragma once

#include <Common/Base.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Vertex3
{
    Vertex3( const float x = 0.f, const float y = 0.f, const float z = 0.f ) // Defaults ( 0.f, 0.f, 0.f )
        : x( x ), y( y ), z( z )
    {
    }

    float x, y, z;
};

struct Color
{
    Color( const float r = 0.f, const float g = 0.f, const float b = 0.f, const float a = 1.f ) // Opaque black default
        : r( r ), g( g ), b( b ), a( a )
    {
    }

    float r, g, b, a;
};