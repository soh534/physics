#pragma once

#pragma once

#include "Common.h"
#include "Shader.h"

int testMergeVbo()
{
    Assert( glfwInit() == GLFW_TRUE, "failed to initialize GLFW" );

    int width = 1024;
    int height = 768;

    GLFWwindow* window = glfwCreateWindow( width, height, "Ogl Behavior Test", nullptr, nullptr );
    Assert( window, "glfwCreateWindow failed" );
    glfwMakeContextCurrent( window );

    Assert( glewInit() == GLEW_OK, "failed to initialize GLEW" );

    Shader* shader = new Shader( "GeomVert.shader", "GeomFrag.shader" );

    Vertex3 verticesA[3] =
    {
        { 0.2f, -0.2f, 0.0f },
        { 0.6f, -0.2f, 0.0f },
        { 0.4f, 0.2f, 0.0f }
    };


    Color colorsA[3] =
    {
        { 1.f, 0.f, 0.f, 1.f },
        { 1.f, 0.f, 0.f, 1.f },
        { 0.f, 0.f, 1.f, 1.f }
    };

    Vertex3 verticesB[3] =
    {
        { -0.6f, -0.2f, 0.0f },
        { -0.2f, -0.2f, 0.0f },
        { -0.4f, 0.2f, 0.0f }
    };


    Color colorsB[3] =
    {
        { 0.f, 0.f, 1.f, 1.f },
        { 0.f, 0.f, 1.f, 1.f },
        { 0.f, 1.f, 0.f, 1.f }
    };

    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    GLuint vbo;
    {
        // Put vertex positions and color into same VBO
        // Not interleaved but in adjacent blocks (VVNNCC)
        const int numVerts = 6;

        glGenBuffers( 1, &vbo );

        glBindBuffer( GL_ARRAY_BUFFER, vbo );
        glBufferData( GL_ARRAY_BUFFER, (sizeof( Vertex3 ) + sizeof( Color )) * numVerts, nullptr, GL_DYNAMIC_DRAW );
        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
        glEnableVertexAttribArray( 0 );

        glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof( Vertex3 ) * numVerts) );
        glEnableVertexAttribArray( 1 );
    }
    glBindVertexArray( 0 );

    {
        // Update buffer per-triangle basis, use 6 vertices and 6 colors
        glBindBuffer( GL_ARRAY_BUFFER, vbo );

        // Vertex positions
        glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( verticesA ), verticesA );
        glBufferSubData( GL_ARRAY_BUFFER, sizeof( verticesA ), sizeof( verticesB ), verticesB );

        // Colors
        glBufferSubData( GL_ARRAY_BUFFER, sizeof( verticesA ) + sizeof( verticesB ), sizeof( colorsA ), colorsA );
        glBufferSubData( GL_ARRAY_BUFFER, sizeof( verticesA ) + sizeof( verticesB ) + sizeof( colorsA ), sizeof( colorsB ), colorsB );
    }

    while ( !glfwWindowShouldClose( window ) )
    {
        glClearColor( 0.f, 0.f, 0.f, 1.f ); // Black, full opacity
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        shader->use();
        shader->setMat4( "projection", glm::mat4( 1.f ) );
        shader->setMat4( "view", glm::mat4( 1.f ) );

        glBindVertexArray( vao );

        // Triangle A model
        glm::mat4 modelA( 1.f );
        modelA = glm::rotate( modelA, glm::radians( -30.f ), glm::vec3( 0.f, 0.f, 1.f ) );
        shader->setMat4( "model", modelA );
        glDrawArrays( GL_TRIANGLES, 0, 3 );

        // Triangle B model
        glm::mat4 modelB( 1.f );
        modelB = glm::rotate( modelB, glm::radians( 60.f ), glm::vec3( 1.f, 0.f, 0.f ) );
        shader->setMat4( "model", modelB );
        glDrawArrays( GL_TRIANGLES, 3, 3 );
        glBindVertexArray( 0 );

        glfwSwapBuffers( window );
        glfwPollEvents();
    }

    glfwDestroyWindow( window );

    glfwTerminate();

    exit( EXIT_SUCCESS );

    return 0;
}
