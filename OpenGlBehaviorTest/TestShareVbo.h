#pragma once

#include "Common.h"
#include "Shader.h"

int testShareVbo()
{
    Assert( glfwInit() == GLFW_TRUE, "failed to initialize GLFW" );

    int width = 1024;
    int height = 768;

    GLFWwindow* window = glfwCreateWindow( width, height, "Ogl Behavior Test", nullptr, nullptr );
    Assert( window, "glfwCreateWindow failed" );
    glfwMakeContextCurrent( window );

    Assert( glewInit() == GLEW_OK, "failed to initialize GLEW" );

    Shader* shader = new Shader( "GeomVert.shader", "GeomFrag.shader" );

    Vertex3 verticesAandB[6] =
    {
        { 0.2f, -0.2f, 0.0f }, // Vertices for A
        { 0.6f, -0.2f, 0.0f },
        { 0.4f, 0.2f, 0.0f },
        { -0.6f, -0.2f, 0.0f }, // Vertices for B
        { -0.2f, -0.2f, 0.0f },
        { -0.4f, 0.2f, 0.0f }
    };

    Color colorsAandB[6] =
    {
        { 1.f, 0.f, 0.f, 1.f }, // Colors for A
        { 1.f, 0.f, 0.f, 1.f },
        { 0.f, 0.f, 1.f, 1.f },
        { 0.f, 0.f, 1.f, 1.f }, // Colors for B
        { 0.f, 0.f, 1.f, 1.f },
        { 0.f, 1.f, 0.f, 1.f }
    };

    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );
    {
        // Triangle A and B
        {
            GLuint vbo[2];
            glGenBuffers( 2, vbo );

            glBindBuffer( GL_ARRAY_BUFFER, vbo[0] );
            glBufferData( GL_ARRAY_BUFFER, sizeof( verticesAandB ), verticesAandB, GL_STATIC_DRAW );
            glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, nullptr );
            glEnableVertexAttribArray( 0 );

            glBindBuffer( GL_ARRAY_BUFFER, vbo[1] );
            glBufferData( GL_ARRAY_BUFFER, sizeof( colorsAandB ), colorsAandB, GL_STATIC_DRAW );
            glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, 0, nullptr );
            glEnableVertexAttribArray( 1 );
        }
    }
    glBindVertexArray( 0 );

    // You can put geometries with same vertex layout to same vbo
    // Update the model matrices per geometry
    // Do draw-calls on geometry index basis

    while ( !glfwWindowShouldClose( window ) )
    {
        glfwPollEvents();

        glClearColor( 0.f, 0.f, 0.f, 1.f ); // Black, full opacity
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        shader->use();
        shader->setMat4( "projection", glm::mat4( 1.f ) );
        shader->setMat4( "view", glm::mat4( 1.f ) );

        glBindVertexArray( vao );

        // Triangle A model
        //glm::mat4 modelA( 1.f );
        //modelA = glm::rotate( modelA, glm::radians( -30.f ), glm::vec3( 0.f, 0.f, 1.f ) );
        //shader->setMat4( "model", modelA );
        //glDrawArrays( GL_TRIANGLES, 0, 3 );

        // Triangle B model
        glm::mat4 modelB( 1.f );
        modelB = glm::rotate( modelB, glm::radians( 60.f ), glm::vec3( 1.f, 0.f, 0.f ) );
        shader->setMat4( "model", modelB );
        glDrawArrays( GL_TRIANGLES, 3, 3 );
        glBindVertexArray( 0 );

        glfwSwapBuffers( window );
    }

    glfwDestroyWindow( window );

    glfwTerminate();

    exit( EXIT_SUCCESS );

    return 0;
}
