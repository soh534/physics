#pragma once

#include "Common.h"
#include "Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

int testIbo()
{
    Assert( glfwInit() == GLFW_TRUE, "failed to initialize GLFW" );

    int width = 1024;
    int height = 768;

    GLFWwindow* window = glfwCreateWindow( width, height, "Ogl Behavior Test", nullptr, nullptr );
    Assert( window, "glfwCreateWindow failed" );
    glfwMakeContextCurrent( window );

    Assert( glewInit() == GLEW_OK, "failed to initialize GLEW" );

    Shader* shader = new Shader( "GeomVertWithNorm.shader", "GeomFragWithNorm.shader" );

    // 
    float verts[(3+3+4) * 24] =
    {
        // Vertices           // Normals           // Colors
        -1.f, -1.f,  1.f,     0.f,  0.f,  1.f,     1.f, 0.f, 0.f, 1.f,
         1.f, -1.f,  1.f,     0.f,  0.f,  1.f,     1.f, 0.f, 0.f, 1.f,
         1.f,  1.f,  1.f,     0.f,  0.f,  1.f,     1.f, 0.f, 0.f, 1.f,
        -1.f,  1.f,  1.f,     0.f,  0.f,  1.f,     1.f, 0.f, 0.f, 1.f,

         1.f,  1.f,  1.f,     1.f,  0.f,  0.f,     1.f, 0.f, 0.f, 1.f,
         1.f,  1.f, -1.f,     1.f,  0.f,  0.f,     1.f, 0.f, 0.f, 1.f,
         1.f, -1.f, -1.f,     1.f,  0.f,  0.f,     1.f, 0.f, 0.f, 1.f,
         1.f, -1.f,  1.f,     1.f,  0.f,  0.f,     1.f, 0.f, 0.f, 1.f,

        -1.f, -1.f, -1.f,     0.f,  0.f, -1.f,     1.f, 0.f, 0.f, 1.f,
         1.f, -1.f, -1.f,     0.f,  0.f, -1.f,     1.f, 0.f, 0.f, 1.f,
         1.f,  1.f, -1.f,     0.f,  0.f, -1.f,     1.f, 0.f, 0.f, 1.f,
        -1.f,  1.f, -1.f,     0.f,  0.f, -1.f,     1.f, 0.f, 0.f, 1.f,

        -1.f, -1.f, -1.f,    -1.f,  0.f,  0.f,     1.f, 0.f, 0.f, 1.f,
        -1.f, -1.f,  1.f,    -1.f,  0.f,  0.f,     1.f, 0.f, 0.f, 1.f,
        -1.f,  1.f,  1.f,    -1.f,  0.f,  0.f,     1.f, 0.f, 0.f, 1.f,
        -1.f,  1.f, -1.f,    -1.f,  0.f,  0.f,     1.f, 0.f, 0.f, 1.f,

         1.f,  1.f,  1.f,     0.f,  1.f,  0.f,     1.f, 0.f, 0.f, 1.f,
        -1.f,  1.f,  1.f,     0.f,  1.f,  0.f,     1.f, 0.f, 0.f, 1.f,
        -1.f,  1.f, -1.f,     0.f,  1.f,  0.f,     1.f, 0.f, 0.f, 1.f,
         1.f,  1.f, -1.f,     0.f,  1.f,  0.f,     1.f, 0.f, 0.f, 1.f,

        -1.f, -1.f, -1.f,     0.f, -1.f,  0.f,     1.f, 0.f, 0.f, 1.f,
         1.f, -1.f, -1.f,     0.f, -1.f,  0.f,     1.f, 0.f, 0.f, 1.f,
         1.f, -1.f,  1.f,     0.f, -1.f,  0.f,     1.f, 0.f, 0.f, 1.f,
        -1.f, -1.f,  1.f,     0.f, -1.f,  0.f,     1.f, 0.f, 0.f, 1.f
    };

    GLushort indices[36] =
    { 
        0, 1, 2, 0, 2, 3,       //front
        4, 5, 6, 4, 6, 7,       //right
        8, 9, 10, 8, 10, 11,    //back
        12, 13, 14, 12, 14, 15, //left
        16, 17, 18, 16, 18, 19, //upper
        20, 21, 22, 20, 22, 23  //bottom
    }; 

    // Interleave vertex buffer, fill data
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    GLuint vbo, ibo;
    {
        glGenBuffers( 1, &vbo );
        glBindBuffer( GL_ARRAY_BUFFER, vbo );
        glBufferData( GL_ARRAY_BUFFER, sizeof( verts ), &verts[0], GL_STATIC_DRAW );

        glGenBuffers( 1, &ibo );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( indices ), &indices[0], GL_STATIC_DRAW );

        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( float ) * 10, (void*)0 );
        glEnableVertexAttribArray( 0 ); // Position

        glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof( float ) * 10, (void*)(3 * sizeof( float )) );
        glEnableVertexAttribArray( 1 ); // Normal

        glVertexAttribPointer( 2, 4, GL_FLOAT, GL_FALSE, sizeof( float ) * 10, (void*)(6 * sizeof( float )) );
        glEnableVertexAttribArray( 2 ); // Color
    }
    glBindVertexArray( 0 );

    // Camera
    glm::vec3 cameraPos( 0.f, 0.f, 5.f );
    glm::vec3 cameraDir( 0.f, 0.f, -1.f );
    glm::vec3 cameraUp( 0.f, 1.f, 0.f );

    glm::vec3 lightColor( 1.f, 1.f, 1.f );
    glm::vec3 lightPos = cameraPos;
    glm::vec3 backColor( 0.f, 0.f, 0.f );

    float fov = 45.f;
    float nearPlane = .01f;
    float farPlane = 100.f;

    glm::mat4 projection = glm::perspective( glm::radians( fov ), (float)width / (float)height, nearPlane, farPlane );
    glm::mat4 view = glm::lookAt( cameraPos, cameraPos + cameraDir, cameraUp );

    while ( !glfwWindowShouldClose( window ) )
    {
        glClearColor( backColor.x, backColor.y, backColor.z, 1.f ); // Black, full opacity
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        shader->use();
        shader->setMat4( "projection", projection );
        shader->setMat4( "view", view );

        shader->setVec3( "lightColor", lightColor );
        shader->setVec3( "lightPos", lightPos );
        shader->setVec3( "viewPos", cameraPos );

        glBindVertexArray( vao );

        glm::mat4 model = glm::translate( glm::mat4(), glm::vec3( 0.f, 0.f, 0.f ) );
        model = glm::rotate( model, glm::pi<float>() / 4.f, glm::vec3( 1.f, 1.f, 1.f ) );
        shader->setMat4( "model", model );
        glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0 );
        glBindVertexArray( 0 );

        glfwSwapBuffers( window );
        glfwPollEvents();
    }

    glfwDestroyWindow( window );

    glfwTerminate();

    delete shader;

    exit( EXIT_SUCCESS );

    return 0;
}
