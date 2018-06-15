#pragma once

#include "Common.h"
#include "Shader.h"

int testReuseVertexAttribIndex()
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
        { 0.f, 1.f, 0.f, 1.f },
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
        { 1.f, 0.f, 0.f, 1.f },
        { 0.f, 1.f, 0.f, 1.f },
        { 0.f, 0.f, 1.f, 1.f }
    };

    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );
    {
        // Triangle A
        {
            GLuint vbo[2];
            glGenBuffers( 2, vbo );

            glBindBuffer( GL_ARRAY_BUFFER, vbo[0] );
            glBufferData( GL_ARRAY_BUFFER, sizeof( verticesA ), verticesA, GL_STATIC_DRAW );
            glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, nullptr );
            glEnableVertexAttribArray( 0 );

            glBindBuffer( GL_ARRAY_BUFFER, vbo[1] );
            glBufferData( GL_ARRAY_BUFFER, sizeof( colorsA ), colorsA, GL_STATIC_DRAW );
            glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, 0, nullptr );
            glEnableVertexAttribArray( 1 );
        }

        // Triangle B
        {
            GLuint vbo[2];
            glGenBuffers( 2, vbo );

            glBindBuffer( GL_ARRAY_BUFFER, vbo[0] );
            glBufferData( GL_ARRAY_BUFFER, sizeof( verticesB ), verticesB, GL_STATIC_DRAW );
            glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, nullptr );
            glEnableVertexAttribArray( 0 );

            glBindBuffer( GL_ARRAY_BUFFER, vbo[1] );
            glBufferData( GL_ARRAY_BUFFER, sizeof( colorsB ), colorsB, GL_STATIC_DRAW );
            glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, 0, nullptr );
            glEnableVertexAttribArray( 1 );
        }
    }
    glBindVertexArray( 0 );

    // Takeaway:
    // This will not work because triangle B's glVertexAttribPointer calls
    // will overwrite which buffer to read from for position and color attributes,
    // hence only rendering triangle B

    while ( !glfwWindowShouldClose( window ) )
    {
        glfwPollEvents();

        glClearColor( 0.f, 0.f, 0.f, 1.f ); // Black, full opacity
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        shader->use();
        shader->setMat4( "projection", glm::mat4( 1.f ) );
        shader->setMat4( "view", glm::mat4( 1.f ) );
        shader->setMat4( "model", glm::mat4( 1.f ) );

        glBindVertexArray( vao );
        glDrawArrays( GL_TRIANGLES, 0, 3 );
        glBindVertexArray( 0 );

        glfwSwapBuffers( window );
    }

    glfwDestroyWindow( window );

    glfwTerminate();

    exit( EXIT_SUCCESS );

    return 0;
}
