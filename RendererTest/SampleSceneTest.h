#pragma once

#include <vector>
#include <random>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Test
{
    struct RendererObjects
    {
        std::vector<glm::mat4> m_objectModels;
        std::vector<glm::vec3> m_aors;
        std::vector<float> m_rotSpeed;
        std::vector<int> m_objectIds;
    } objs;

    void addObjects( Renderer& renderer )
    {
        std::default_random_engine engine;
        std::uniform_real_distribution<float> posDistr( -10.f, 10.f );
        std::uniform_real_distribution<float> sizeDistr( .5f, 1.f );
        std::uniform_real_distribution<float> colorDistr( 0.f, 1.f );
        std::uniform_real_distribution<float> axisDistr( -1.f, 1.f );
        std::uniform_real_distribution<float> piDivDistr( 30.f, 180.f );

        for ( int i = 0; i < 5; i++ )
        {
            float length = sizeDistr( engine );
            Vertex3 min( -length, -length, -length );
            Vertex3 max( length, length, length );

            glm::mat4 model = glm::mat4( 1.f );
            model = glm::translate( model, glm::vec3( posDistr( engine ), posDistr( engine ), posDistr( engine ) ) );

            Color color( colorDistr( engine ), colorDistr( engine ), colorDistr( engine ) );
            int objId = renderer.addDisplayCuboid( min, max, model, color );
            objs.m_objectIds.push_back( objId );
            objs.m_objectModels.push_back( model );
            objs.m_aors.push_back( glm::vec3( axisDistr( engine ), axisDistr( engine ), axisDistr( engine ) ) );
            objs.m_rotSpeed.push_back( glm::pi<float>() / piDivDistr( engine ) );
        }
    }

    void updateObjects( Renderer& renderer )
    {
        for ( int i = 0; i < objs.m_objectIds.size(); i++ )
        {
            objs.m_objectModels[i] = glm::rotate( objs.m_objectModels[i], objs.m_rotSpeed[i], objs.m_aors[i] );
            renderer.setModel( objs.m_objectIds[i], objs.m_objectModels[i] );
        }
    }
}
