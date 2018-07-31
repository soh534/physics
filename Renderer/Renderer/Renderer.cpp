#include <Common/Base.h>
#include <Common/FileIO.h>

#include "Renderer.h"
#include "Shader.h"
#include "Camera.h"

#include "Thirdparty/Imgui/imgui.h"
#include "Thirdparty/Imgui/imgui_impl_glfw_gl3.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Vertex2::Vertex2( const float x, const float y )
    : x( x ), y( y )
{
}

Vertex3::Vertex3( const float x, const float y, const float z )
    : x( x ), y( y ), z( z )
{
}

Color::Color( float r, float g, float b, float a )
    : r( r ), g( g ), b( b ), a( a )
{
}

void Renderer::DisplayLines::create()
{
    m_shader = new Shader(
        "../Renderer/Renderer/shaders/Line/LineVert.shader",
        "../Renderer/Renderer/shaders/Line/LineFrag.shader"
    );

    glGenVertexArrays( 1, &m_vao );
    glGenBuffers( 2, m_vbo );

    glBindVertexArray( m_vao );
    {
        glBindBuffer( GL_ARRAY_BUFFER, m_vbo[0] );
        glBufferData( GL_ARRAY_BUFFER, sizeof( m_vertices ), m_vertices, GL_DYNAMIC_DRAW );
        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, nullptr );
        glEnableVertexAttribArray( 0 ); // Position

        glBindBuffer( GL_ARRAY_BUFFER, m_vbo[1] );
        glBufferData( GL_ARRAY_BUFFER, sizeof( m_color ), m_color, GL_DYNAMIC_DRAW );
        glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, 0, nullptr );
        glEnableVertexAttribArray( 1 ); // Color
    }
    glBindVertexArray( 0 );

    m_numVerts = 0;
}

void Renderer::DisplayLines::writeBufferLine( const Vertex3 a, const Vertex3 b, const Color color )
{
    Assert( m_numVerts < MAX_NUM_LINES * 2, "Max # of lines exceeded." );
    m_vertices[m_numVerts] = a;
    m_color[m_numVerts] = color;
    m_vertices[m_numVerts + 1] = b;
    m_color[m_numVerts + 1] = color;

    m_numVerts += 2;
}

void Renderer::DisplayLines::render( const glm::mat4& projection, const glm::mat4& view )
{
    // Single draw call for all lines.
    m_shader->use();
    m_shader->setMat4( "projection", projection );
    m_shader->setMat4( "view", view );

    glBindVertexArray( m_vao );
    {
        glBindBuffer( GL_ARRAY_BUFFER, m_vbo[0] );
        glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( Vertex3 ) * m_numVerts, m_vertices );

        glBindBuffer( GL_ARRAY_BUFFER, m_vbo[1] );
        glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( Color ) * m_numVerts, m_color );

        glDrawArrays( GL_LINES, 0, m_numVerts );
    }
    glBindVertexArray( 0 );

    m_numVerts = 0;
}

void Renderer::DisplayCuboids::create()
{
    m_shader = new Shader(
        "../Renderer/Renderer/shaders/Geometry/GeomVert.shader",
        "../Renderer/Renderer/shaders/Geometry/GeomFrag.shader"
    );

    m_cuboids = new ArrayFreeList<Cuboid>( NUM_INITIAL_MAX_CUBOIDS );

    glGenVertexArrays( 1, &m_vao );
    glBindVertexArray( m_vao );

    glGenBuffers( 1, &m_vbo );
    glBindBuffer( GL_ARRAY_BUFFER, m_vbo );

    const int numBytesVbo = m_cuboids->getCapacity() * sizeof( Cuboid::m_vertices );
    glBufferData( GL_ARRAY_BUFFER, numBytesVbo, nullptr, GL_DYNAMIC_DRAW );

    setVertexAttributes();

    glBindVertexArray( 0 );
}

int Renderer::DisplayCuboids::writeBufferCuboid( const float* vertices, const glm::mat4& model )
{
    Cuboid cuboid;
    cuboid.m_model = model;
    cuboid.set( vertices );

    if ( m_cuboids->isFull() )
    {
        expandBuffer();
    }

    const int index = m_cuboids->add( cuboid );

    // Write positions, normals, colors interleaved
    glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
    glBufferSubData( GL_ARRAY_BUFFER, index * sizeof( Cuboid::m_vertices ), sizeof( Cuboid::m_vertices ), cuboid.getVertices() );

    return index;
}

void Renderer::DisplayCuboids::clearBufferCuboid( int index )
{
    m_cuboids->remove( index );
}

void Renderer::DisplayCuboids::setVertexAttributes()
{
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, Cuboid::NUM_FLOATS_VERT * sizeof( float ), (void*)0 );
    glEnableVertexAttribArray( 0 ); // Position

    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, Cuboid::NUM_FLOATS_VERT * sizeof( float ), (void*)(3 * sizeof( float )) );
    glEnableVertexAttribArray( 1 ); // Normal

    glVertexAttribPointer( 2, 4, GL_FLOAT, GL_FALSE, Cuboid::NUM_FLOATS_VERT * sizeof( float ), (void*)(6 * sizeof( float )) );
    glEnableVertexAttribArray( 2 ); // Color
}

void Renderer::DisplayCuboids::expandBuffer()
{
    // Expand array freelist, allocate new vertex buffer, copy old contents to new buffer
    const int numBytesOriginalVbo = m_cuboids->getCapacity() * sizeof( Cuboid::m_vertices );

    m_cuboids->expand();
    const int numBytesNewVbo = m_cuboids->getCapacity() * sizeof( Cuboid::m_vertices );

    GLuint newVbo;
    glGenBuffers( 1, &newVbo );

    glBindBuffer( GL_ARRAY_BUFFER, newVbo );
    glBufferData( GL_ARRAY_BUFFER, numBytesNewVbo, nullptr, GL_DYNAMIC_DRAW );

    glBindBuffer( GL_COPY_READ_BUFFER, m_vbo );
    glBindBuffer( GL_COPY_WRITE_BUFFER, newVbo );
    glCopyBufferSubData( GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, numBytesOriginalVbo );

    glBindBuffer( GL_COPY_READ_BUFFER, 0 );
    glBindBuffer( GL_COPY_WRITE_BUFFER, 0 );

    glDeleteBuffers( 1, &m_vbo );

    m_vbo = newVbo;

    // Set attributes for newly allocated buffer
    glBindVertexArray( m_vao );
    setVertexAttributes();
    glBindVertexArray( 0 );
}

void Renderer::DisplayCuboids::render( const glm::mat4& projection, const glm::mat4& view, const LightSource& lightSource, const glm::vec3 cameraPos )
{
    m_shader->use();
    m_shader->setMat4( "projection", projection );
    m_shader->setMat4( "view", view );

    m_shader->setVec3( "lightColor", lightSource.m_color );
    //m_shader->setVec3( "lightPos", lightSource.m_pos );
    m_shader->setVec3( "lightPos", cameraPos );
    m_shader->setVec3( "viewPos", cameraPos );

    glBindVertexArray( m_vao );

    for ( int cuboidIdx = 0; cuboidIdx < m_cuboids->getSize(); cuboidIdx++ )
    {
        if ( m_cuboids->isUsed( cuboidIdx ) )
        {
            Cuboid& cuboid = (*m_cuboids)(cuboidIdx);
            m_shader->setMat4( "model", cuboid.m_model );
            glDrawArrays( GL_TRIANGLES, cuboidIdx * Cuboid::NUM_VERTS, Cuboid::NUM_VERTS );
        }
    }

    glBindVertexArray( 0 );
}

void Renderer::DisplayCuboids::setModel( int index, const glm::mat4& model )
{
    // TODO: Check isValid()
    Cuboid& cuboid = (*m_cuboids)(index);
    cuboid.m_model = model;
}

Renderer::Renderer()
{

}

Renderer::~Renderer()
{
    delete m_camera;
}

int Renderer::init( GLFWwindow* window, const RendererCinfo& cinfo )
{
    m_window = window;

    int width, height;
    glfwGetFramebufferSize( window, &width, &height );

    m_backColor = cinfo.m_backColor;

    m_camera = new Camera( cinfo.m_cameraPos, cinfo.m_cameraDir, cinfo.m_cameraUp );

    m_projection = glm::perspective( glm::radians( cinfo.m_fov ), (float)width / (float)height, cinfo.m_nearPlane, cinfo.m_farPlane );
    m_view = glm::lookAt( m_camera->getPos(), m_camera->getPos() + m_camera->getDir(), m_camera->getUp() );

    m_displayLines.create();
    m_displayCuboids.create();

    m_lightSource.m_color = cinfo.m_lightColor;
    m_lightSource.m_pos = cinfo.m_lightPos;

    glEnable( GL_DEPTH_TEST );

    ImGui::CreateContext();
    ImGui_ImplGlfwGL3_Init( window, true );

    return 0;
}

void Renderer::prestep()
{
    glClearColor( m_backColor.x, m_backColor.y, m_backColor.z, 1.f ); // Black, full opacity
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    ImGui_ImplGlfwGL3_NewFrame();

    // Not frame-persistent, must be called every frame
    ImGui::SetNextWindowPos( ImVec2( 0, 0 ) );

    int width, height;
    glfwGetFramebufferSize( m_window, &width, &height );

    ImGui::SetNextWindowSize( ImVec2( (float)width, (float)height ) );
    ImGui::SetNextWindowBgAlpha( 0.f );
}

void Renderer::render()
{
    // Update view matrix, flush display lines & geoms, step Imgui
    m_view = glm::lookAt( m_camera->getPos(), m_camera->getPos() + m_camera->getDir(), m_camera->getUp() );

    m_displayLines.render( m_projection, m_view );
    m_displayCuboids.render( m_projection, m_view, m_lightSource, m_camera->getPos() );

    ImGui::Render();
    ImGui_ImplGlfwGL3_RenderDrawData( ImGui::GetDrawData() );
}

int Renderer::terminate()
{
    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();

    return 0;
}

void Renderer::addDisplayLine( const Vertex3 a, const Vertex3 b, const Color color )
{
    m_displayLines.writeBufferLine( a, b, color );
}

/*
void Renderer::drawTriangle( const Triangle& tri, const Color color )
{
m_displayCuboids.addDisplayCuboid( &tri, 1, glm::mat4( 1.f ), color );
}
*/


int Renderer::addDisplayCuboid( const Vertex3 min, const Vertex3 max, const glm::mat4& model, const Color color )
{
    // TODO: add a function which just accepts an array of display cuboid vertices
    // Leave the vertex preparation to user

    // Prepare 8 corners from cube's min-max extents
    Vertex3 corners[8] =
    {
        { min.x, min.y, min.z }, // A 0 ---
        { max.x, min.y, min.z }, // B 1 +--
        { max.x, max.y, min.z }, // C 2 ++-
        { min.x, max.y, min.z }, // D 3 -+-
        { min.x, min.y, max.z }, // E 4 --+
        { max.x, min.y, max.z }, // F 5 +-+
        { max.x, max.y, max.z }, // G 6 +++
        { min.x, max.y, max.z }  // H 7 -++
    };

    // Prepare 36 positions forming 12 triangles forming cuboid
    Vertex3 positions[DisplayCuboids::Cuboid::NUM_VERTS] =
    {
        corners[0], corners[3], corners[2], // ADC 032
        corners[2], corners[1], corners[0], // CBA 210

        corners[4], corners[5], corners[6], // EFG 456
        corners[6], corners[7], corners[4], // GHE 674

        corners[7], corners[3], corners[0], // HDA 730
        corners[0], corners[4], corners[7], // AEH 047

        corners[6], corners[5], corners[1], // GFB 651
        corners[1], corners[2], corners[6], // BCG 126

        corners[0], corners[1], corners[5], // ABF 015
        corners[5], corners[4], corners[0], // FEA 540

        corners[6], corners[2], corners[3], // GCD 623
        corners[3], corners[7], corners[6]  // DHG 376
    };

    // Prepare 36 normals for each positions
    Vertex3 normals[DisplayCuboids::Cuboid::NUM_VERTS];

    for ( int vidx = 0; vidx < DisplayCuboids::Cuboid::NUM_VERTS; vidx += 3 )
    {
        Vertex3 ab = positions[vidx + 1] - positions[vidx];
        Vertex3 ac = positions[vidx + 2] - positions[vidx];
        normals[vidx] = ab.cross( ac );
        normals[vidx + 1] = ab.cross( ac );
        normals[vidx + 2] = ab.cross( ac );
    }

    // Prepare interleaved floats
    float vertices[DisplayCuboids::Cuboid::NUM_FLOATS] = { 0 };
    for ( int vidx = 0; vidx < DisplayCuboids::Cuboid::NUM_VERTS; vidx++ )
    {
        int offset = vidx * DisplayCuboids::Cuboid::NUM_FLOATS_VERT;
        vertices[offset] = positions[vidx].x;
        vertices[offset + 1] = positions[vidx].y;
        vertices[offset + 2] = positions[vidx].z;
        vertices[offset + 3] = normals[vidx].x;
        vertices[offset + 4] = normals[vidx].y;
        vertices[offset + 5] = normals[vidx].z;
        vertices[offset + 6] = color.r;
        vertices[offset + 7] = color.g;
        vertices[offset + 8] = color.b;
        vertices[offset + 9] = color.a;
    }

    const int index = m_displayCuboids.writeBufferCuboid( vertices, model );

    return index;
}

void Renderer::removeDisplayCuboid( int index )
{
    m_displayCuboids.clearBufferCuboid( index );
}

void Renderer::drawText2d( const Vertex2 pos, const Color color, const char* string, ... )
{
    va_list arg;
    va_start( arg, string );
    ImGui::Begin( "Overlay", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar );

    ImVec2 posImvec( pos.x, pos.y );
    ImGui::SetCursorPos( posImvec ); // Window-coordinate system (pixel-based, top-left origin)

    ImColor colorIm( color.r, color.g, color.b, color.a );
    ImGui::TextColoredV( colorIm, string, arg );
    ImGui::End();
    va_end( arg );
}

void Renderer::drawText3d( const Vertex3 pos, const Color color, const char* string, ... )
{
    // TODO: make this into a function
    // Transform to clip-space
    glm::vec4 posClip = m_projection * m_view * glm::vec4( pos.x, pos.y, pos.z, 1.f );

    // Perspective division to NDC
    if ( posClip.w < 0.f )
    {
        return;
    }

    posClip.x /= posClip.w;
    posClip.y /= posClip.w;

    // Viewport transformation to window-coordinates (pixel-based, top-left origin)
    int width, height;
    glfwGetFramebufferSize( m_window, &width, &height );
    ImVec2 posWindow( (posClip.x + 1) * width / 2, (posClip.y - 1) * -height / 2 );

    va_list arg;
    va_start( arg, string );
    ImGui::Begin( "Overlay", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar );
    ImGui::SetCursorPos( posWindow );

    ImColor colorIm( color.r, color.g, color.b, color.a );
    ImGui::TextColoredV( colorIm, string, arg );
    ImGui::End();
    va_end( arg );
}

void Renderer::setModel( int index, const glm::mat4& model )
{
    // TODO: Separate indices between different shape buffers, choose multiple buffers from here.
    m_displayCuboids.setModel( index, model );
}
