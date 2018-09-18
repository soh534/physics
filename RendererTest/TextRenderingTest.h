#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Test
{
    glm::vec3 pos;

    void addObjects( Renderer& renderer )
    {
        pos = glm::vec3( 0.f, 0.f, 0.f );

        Vertex3 min( -.01f, -.01f, -.01f );
        Vertex3 max( .01f, .01f, .01f );

        glm::mat4 model = glm::translate( glm::mat4(), pos );
        model = glm::rotate( model, glm::pi<float>() / 4.f, glm::vec3( 1.f, 1.f, 1.f ) );
        renderer.addDisplayCuboid( min, max, model, Color( 1.f, 1.f, 1.f ) );
    }

    void updateObjects( Renderer& renderer )
    {
        //renderer.drawText2d( Vertex2( pos.x, pos.y ), Color( 1.f, 1.f, 1.f ), "%f, %f", pos.x, pos.y );

        renderer.drawText3d( Vertex3( pos.x, pos.y, pos.z ), Color( 1.f, 1.f, 1.f ), "%f, %f, %f", pos.x, pos.y, pos.z );
    }
}
