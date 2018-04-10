#include <Common/Base.h>

#include <Renderer/Renderer/Renderer.h>
#include <Renderer/Renderer/Shader.h>
#include <Renderer/Renderer/Camera.h>
#include <Common/FileIO.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <sstream>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

typedef Renderer::DisplayLine DisplayLine;
typedef Renderer::DisplayTriangle DisplayTriangle;
typedef Renderer::DisplayText DisplayText;

struct Character
{
	GLuint m_textureId;
	Vector4 m_size;
	Vector4 m_bearing;
	FT_Pos m_advance;
};

std::map<GLchar, Character> Characters;

Renderer::Renderable::Renderable( unsigned int color )
	: m_color( color )
{

}

DisplayLine::DisplayLine( const Vector4& a, const Vector4& b, unsigned int color ) 
	: m_a( a ), m_b( b ), Renderable( color )
{

}

void Renderer::renderLine( const DisplayLine& line ) const
{
	float vertices[] =
	{
		line.m_a( 0 ), line.m_a( 1 ), line.m_b( 0 ), line.m_b( 1 )
	};

	float color[] =
	{
		(line.m_color & 0xff) / 255.f,
		((line.m_color >> 8) & 0xff) / 255.f,
		((line.m_color >> 16) & 0xff) / 255.f,
		((line.m_color >> 24) & 0xff) / 255.f,
		(line.m_color & 0xff) / 255.f,
		((line.m_color >> 8) & 0xff) / 255.f,
		((line.m_color >> 16) & 0xff) / 255.f,
		((line.m_color >> 24) & 0xff) / 255.f
	};

	m_lineShader->use();

	m_lineShader->setMat4( "projection", m_projection );

	glBindVertexArray( m_lineVAO );
	glBindBuffer( GL_ARRAY_BUFFER, m_lineVBO[0] );
	glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( vertices ), vertices );

	glBindBuffer( GL_ARRAY_BUFFER, m_lineVBO[1] );
	glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( color ), color );

	glDrawArrays( GL_LINE_STRIP, 0, 2 );

	glBindVertexArray( 0 );
}

DisplayTriangle::DisplayTriangle( const Vector4& a, const Vector4& b, const Vector4& c, unsigned int color )
	: m_a( a ), m_b( b ), m_c( c ), Renderable( color )
{

}

void Renderer::renderTriangle( const DisplayTriangle& tri ) const
{
	float vertices[] =
	{
		tri.m_a( 0 ), tri.m_a( 1 ), tri.m_a( 2 ),
		tri.m_b( 0 ), tri.m_b( 1 ), tri.m_b( 2 ),
		tri.m_c( 0 ), tri.m_c( 1 ), tri.m_c( 2 ),
	};

	float color[] =
	{
		(tri.m_color & 0xff) / 255.f,
		((tri.m_color >> 8) & 0xff) / 255.f,
		((tri.m_color >> 16) & 0xff) / 255.f,
		((tri.m_color >> 24) & 0xff) / 255.f,
		(tri.m_color & 0xff) / 255.f,
		((tri.m_color >> 8) & 0xff) / 255.f,
		((tri.m_color >> 16) & 0xff) / 255.f,
		((tri.m_color >> 24) & 0xff) / 255.f,
		(tri.m_color & 0xff) / 255.f,
		((tri.m_color >> 8) & 0xff) / 255.f,
		((tri.m_color >> 16) & 0xff) / 255.f,
		((tri.m_color >> 24) & 0xff) / 255.f
	};

	m_triShader->use();

	m_triShader->setMat4( "projection", m_projection );
	m_triShader->setMat4( "view", m_view );
	m_triShader->setMat4( "model", m_model );

	glBindVertexArray( m_triVAO );
	glBindBuffer( GL_ARRAY_BUFFER, m_triVBO[0] );
	glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( vertices ), vertices );

	glBindBuffer( GL_ARRAY_BUFFER, m_triVBO[1] );
	glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( color ), color );

	glDrawArrays( GL_TRIANGLES, 0, 3 );

	glBindVertexArray( 0 );
}

DisplayText::DisplayText( const std::string& str, const Vector4& pos, const Real scale, unsigned int color )
	: m_str( str ), m_pos( pos ), m_scale( scale ), Renderable( color )
{

}

void Renderer::renderText( const DisplayText& text ) const
{
	m_textShader->use();

	m_textShader->setMat4( "projection", m_projection );
	m_textShader->setMat4( "view", m_view );
	m_textShader->setMat4( "model", m_model );

	m_textShader->setVec3(
		"textColor",
		(float) (text.m_color & 0xff) / 255.f,
		(float) (text.m_color >> 8 & 0xff) / 255.f,
		(float) (text.m_color >> 16 & 0xff) / 255.f );

	glActiveTexture( GL_TEXTURE0 );
	glBindVertexArray( m_textVAO );

	Real x = text.m_pos( 0 );
	Real y = text.m_pos( 1 );
	Real z = text.m_pos( 2 );

	// Iterate through all characters
	std::string::const_iterator c;
	for ( c = text.m_str.begin(); c != text.m_str.end(); c++ )
	{
		Character ch = Characters[*c];

		float xpos = x + ch.m_bearing( 0 ) * text.m_scale;
		float ypos = y - (ch.m_size( 1 ) - ch.m_bearing( 1 )) * text.m_scale;

		float w = ch.m_size( 0 ) * text.m_scale;
		float h = ch.m_size( 1 ) * text.m_scale;
		// Update VBO for each character
		float vertices[6][3] = 
		{
			{ xpos,     ypos + h,   z },
		    { xpos,     ypos,       z },
            { xpos + w, ypos,       z },
    		{ xpos,     ypos + h,   z },
	    	{ xpos + w, ypos,       z },
		    { xpos + w, ypos + h,   z }
		};

		float uv[6][2] = 
		{
			{0.f, 0.f}, {0.f, 1.f}, {1.f, 1.f}, {0.f, 0.f}, {1.f, 1.f}, {1.f, 0.f}
		};

		// Render glyph texture over quad
		glBindTexture( GL_TEXTURE_2D, ch.m_textureId );

		// Update content of VBO memory
		glBindBuffer( GL_ARRAY_BUFFER, m_textVBO[0] );
		glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( vertices ), vertices );

		glBindBuffer( GL_ARRAY_BUFFER, m_textVBO[1] );
		glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( uv ), uv );

		// Render quad
		glDrawArrays( GL_TRIANGLES, 0, 6 );
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.m_advance >> 6) * text.m_scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
	}

	glBindVertexArray( 0 );
	glBindTexture( GL_TEXTURE_2D, 0 );
}

inline unsigned int SetRGBA( unsigned char r, unsigned char g, unsigned char b, unsigned char a )
{
	return (r) | (g << 8) | (b << 16) | (a << 24);
}

GLuint LoadShaders( const char* vertexShaderPath, const char* fragmentShaderPath )
{
	std::string vertShaderSrcStr;
	Assert( FileIO::loadFileStream( vertexShaderPath, vertShaderSrcStr ) == 0, "Failed to read shader" );

	std::string fragShaderSrcStr;
	Assert( FileIO::loadFileStream( fragmentShaderPath, fragShaderSrcStr ) == 0, "Failed to read shader" );

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile vertex shader
	printf( "Compiling vertex shader: %s\n", vertexShaderPath );
	const char* vertShaderSrcPtr = vertShaderSrcStr.c_str();
	GLuint vertShaderId = glCreateShader( GL_VERTEX_SHADER );
	glShaderSource( vertShaderId, 1, &vertShaderSrcPtr, nullptr );
	glCompileShader( vertShaderId );

	glGetShaderiv( vertShaderId, GL_COMPILE_STATUS, &Result );
	glGetShaderiv( vertShaderId, GL_INFO_LOG_LENGTH, &InfoLogLength );
	if ( InfoLogLength > 0 )
	{
		std::vector<char> VertexShaderErrorMessage( InfoLogLength + 1 );
		glGetShaderInfoLog( vertShaderId, InfoLogLength, nullptr, &VertexShaderErrorMessage[0] );
		printf( "%s\n", &VertexShaderErrorMessage[0] );
	}

	// Compile fragment shader
	printf( "Compiling fragment shader: %s\n", fragmentShaderPath );
	const char* fragShaderSrcPtr = fragShaderSrcStr.c_str();
	GLuint fragShaderId = glCreateShader( GL_FRAGMENT_SHADER );
	glShaderSource( fragShaderId, 1, &fragShaderSrcPtr, nullptr );
	glCompileShader( fragShaderId );

	glGetShaderiv( fragShaderId, GL_COMPILE_STATUS, &Result );
	glGetShaderiv( fragShaderId, GL_INFO_LOG_LENGTH, &InfoLogLength );
	if ( InfoLogLength > 0 )
	{
		std::vector<char> FragmentShaderErrorMessage( InfoLogLength + 1 );
		glGetShaderInfoLog( fragShaderId, InfoLogLength, nullptr, &FragmentShaderErrorMessage[0] );
		printf( "%s\n", &FragmentShaderErrorMessage[0] );
	}

	// Link vertex and fragment shaders
	printf( "Linking program\n" );
	GLuint programId = glCreateProgram();
	glAttachShader( programId, vertShaderId );
	glAttachShader( programId, fragShaderId );
	glLinkProgram( programId );

	glGetProgramiv( programId, GL_LINK_STATUS, &Result );
	glGetProgramiv( programId, GL_INFO_LOG_LENGTH, &InfoLogLength );
	if ( InfoLogLength > 0 )
	{
		std::vector<char> ProgramErrorMessage( InfoLogLength + 1 );
		glGetProgramInfoLog( programId, InfoLogLength, nullptr, &ProgramErrorMessage[0] );
		printf( "%s\n", &ProgramErrorMessage[0] );
	}

	glDetachShader( programId, vertShaderId );
	glDetachShader( programId, fragShaderId );

	glDeleteShader( vertShaderId );
	glDeleteShader( fragShaderId );

	return programId;
}

Renderer::Renderer()
{

}

Renderer::~Renderer()
{
	delete m_camera;
	delete m_lineShader;
	delete m_textShader;
}

int Renderer::init( int width, int height )
{
	GLenum err = glewInit();
	Assert( err == GLEW_OK, "failed to initialize GLEW" );

	m_left = -width * 0.5;
	m_right = width * 0.8;
	m_bottom = -height * 0.5;
	m_top = height * 0.8;

	m_camera = new Camera();

	//m_projection = glm::ortho( (float) m_left, (float) m_right, (float) m_bottom, (float) m_top, -1.f, 1.f );
	m_projection = glm::perspective( glm::radians( 90.f ), (float) (m_right - m_left) / (float) (m_top - m_bottom), 0.1f, 1000.f );
	//m_view = glm::mat4( 1.f );
	updateView();
	//m_view = glm::translate( m_view, glm::vec3( 0.f, 0.f, -500.f ) );
	m_model = glm::mat4( 1.f );
	//m_model = glm::rotate( m_model, glm::radians( -55.f ), glm::vec3( 1.f, 0.f, 0.f ) );

	m_lineShader = new Shader( "../Renderer/Renderer/shaders/VertexShader.shader", "../Renderer/Renderer/shaders/FragmentShader.shader" );

	// TODO: re-write so vertices in buffer is static
	// TODO: re-evaluate diference between 2x glGenVertexArrays() or glGenVertexArrays(2, )
	glGenVertexArrays( 1, &m_lineVAO );
	glGenBuffers( 2, m_lineVBO );

	glBindVertexArray( m_lineVAO );
	{
		glBindBuffer( GL_ARRAY_BUFFER, m_lineVBO[0] );
		glBufferData( GL_ARRAY_BUFFER, sizeof( GL_FLOAT ) * 4, nullptr, GL_DYNAMIC_DRAW );
		glEnableVertexAttribArray( 0 ); // Position
		glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, nullptr );

		glBindBuffer( GL_ARRAY_BUFFER, m_lineVBO[1] );
		glBufferData( GL_ARRAY_BUFFER, sizeof( GL_FLOAT ) * 8, nullptr, GL_DYNAMIC_DRAW );
		glEnableVertexAttribArray( 1 ); // Color
		glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, 0, nullptr );
	}
	glBindVertexArray( 0 );

	m_triShader = new Shader( "../Renderer/Renderer/shaders/TriVertShader.shader", "../Renderer/Renderer/shaders/TriFragShader.shader" );

	glGenVertexArrays( 1, &m_triVAO );
	glGenBuffers( 2, m_triVBO );

	glBindVertexArray( m_triVAO );
	{
		glBindBuffer( GL_ARRAY_BUFFER, m_triVBO[0] );
		glBufferData( GL_ARRAY_BUFFER, sizeof( GL_FLOAT ) * 9, nullptr, GL_DYNAMIC_DRAW );
		glEnableVertexAttribArray( 0 ); // Position
		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

		glBindBuffer( GL_ARRAY_BUFFER, m_triVBO[1] );
		glBufferData( GL_ARRAY_BUFFER, sizeof( GL_FLOAT ) * 12, nullptr, GL_DYNAMIC_DRAW );
		glEnableVertexAttribArray( 1 ); // Color
		glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, 0, nullptr );
	}
	glBindVertexArray( 0 );

	m_textShader = new Shader( "../Renderer/Renderer/shaders/TextVertexShader.shader", "../Renderer/Renderer/shaders/TextFragmentShader.shader" );
	initializeFreeType();

	return 0;
}

int Renderer::initializeFreeType()
{
	FT_Library library = nullptr;
	Assert( !FT_Init_FreeType( &library ), "failed to initialize FreeType" );

	FT_Face face = nullptr;
	Assert( !FT_New_Face( library, "C:\\Windows\\Fonts\\consola.ttf", 0, &face ),
		"failed to load font face" );

	Assert( !FT_Set_Pixel_Sizes( face, 0, 16 ), "failed to define font size" );

	// Pre-load characters
	// Rendering technique taken from
	// https://learnopengl.com/#!In-Practice/Text-Rendering

	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

	for ( GLubyte c = 0; c < 128; c++ )
	{
		Assert( !FT_Load_Char( face, c, FT_LOAD_RENDER ), "failed to load character from font face" );

		// glTexImage2D( GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels );

		GLuint texture;
		glGenTextures( 1, &texture );
		glBindTexture( GL_TEXTURE_2D, texture );
		glTexImage2D( GL_TEXTURE_2D,                 // GLenum target
			0,                             // GLint level
			GL_RED,                        // GLint internalformat
			face->glyph->bitmap.width,     // GLsizei width
			face->glyph->bitmap.rows,      // GLsizei height
			0,                             // GLint border
			GL_RED,                        // GLenum format
			GL_UNSIGNED_BYTE,              // GLenum type
			face->glyph->bitmap.buffer );   // const void *pixels

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		Character character =
		{
			texture,
			Vector4( (Real) face->glyph->bitmap.width, (Real) face->glyph->bitmap.rows ),
			Vector4( (Real) face->glyph->bitmap_left, (Real) face->glyph->bitmap_top ),
			face->glyph->advance.x
		};

		Characters.insert( std::pair<GLchar, Character>( c, character ) );
	}

	FT_Done_Face( face );
	FT_Done_FreeType( library );

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glGenVertexArrays( 1, &m_textVAO );
	glGenBuffers( 2, m_textVBO );

	glBindVertexArray( m_textVAO );
	{
		// TODO: think of a way to interleave vertex positions and uv coordinates
		// TODO: align the vertices to 4x floats
		glBindBuffer( GL_ARRAY_BUFFER, m_textVBO[0] );
		glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * 6 * 3, nullptr, GL_DYNAMIC_DRAW );
		glEnableVertexAttribArray( 0 );
		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

		// TODO: consider GL_STATIC_DRAW for uv coordinates
		glBindBuffer( GL_ARRAY_BUFFER, m_textVBO[1] );
		glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * 6 * 2, nullptr, GL_DYNAMIC_DRAW );
		glEnableVertexAttribArray( 1 );
		glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, nullptr );
	}
	glBindVertexArray( 0 );

	return 0;
}

void Renderer::updateView()
{
	const glm::vec3& pos = m_camera->getPos();
	const glm::vec3& dir = m_camera->getDir();
	const glm::vec3& up = m_camera->getUp();
	m_view = glm::lookAt( pos, pos + dir, up );
}

int Renderer::step()
{
	updateView();

	for ( auto i = 0; i < m_displayLines.size(); i++ )
	{
		renderLine( m_displayLines[i] );
	}
	m_displayLines.clear();

	for ( auto i = 0; i < m_displayTris.size(); i++ )
	{
		renderTriangle( m_displayTris[i] );
	}
	m_displayTris.clear();

	for ( auto i = 0; i < m_displayTexts.size(); i++ )
	{
		renderText( m_displayTexts[i] );
	}
	m_displayTexts.clear();

	return 0;
}

void Renderer::getDimensions( float & left, float & right, float & bottom, float & top )
{
	left = m_left;
	right = m_right;
	bottom = m_bottom;
	top = m_top;
}

void Renderer::drawLine( const Vector4& pa, const Vector4& pb, unsigned int color )
{
	m_displayLines.push_back( DisplayLine( pa, pb, color ) );
}

void Renderer::drawCross( const Vector4& pos, const Real rot, const Real len, unsigned int color )
{
	Vector4 needle( len / 2.f, 0.f );

	Vector4 a, b, c, d;

	a = pos + needle.getRotatedDir( rot );
	b = pos + needle.getRotatedDir( rot + 90.f * g_degToRad );
	c = pos + needle.getRotatedDir( rot + 180.f * g_degToRad );
	d = pos + needle.getRotatedDir( rot + 270.f * g_degToRad );

	drawLine( a, c, color );
	drawLine( b, d, color );
}

void Renderer::drawArrow( const Vector4& pos, const Vector4& dir, unsigned int color )
{
	Vector4 dst; dst.setAdd( pos, dir );
	drawLine( pos, dst, color );

	Vector4 dirFrac = dir * .5f;
	Vector4 headLeft = dirFrac.getRotatedDir( 135.f * g_degToRad );
	Vector4 headRight = dirFrac.getRotatedDir( -135.f * g_degToRad );
	Vector4 dstToHeadLeft = dst + headLeft;
	Vector4 dstToHeadRight = dst + headRight;

	drawLine( dst, dstToHeadLeft, color );
	drawLine( dst, dstToHeadRight, color );
}

void Renderer::drawBox( const Vector4& max, const Vector4& min, unsigned int color )
{
	Vector4 upperLeft( min( 0 ), max( 1 ) );
	Vector4 bottomRight( max( 0 ), min( 1 ) );

	drawLine( max, upperLeft, color );
	drawLine( max, bottomRight, color );
	drawLine( min, upperLeft, color );
	drawLine( min, bottomRight, color );
}

void Renderer::drawCircle( const Vector4& pos, const Real radius, unsigned int color )
{
	Real step = 2 * (Real) M_PI * STEP_RENDER_CIRCLE;
	Real full = (2.f + STEP_RENDER_CIRCLE) * M_PI;

	for ( Real i = step; i < full; i += step )
	{
		Vector4 na, nb;
		na.set( radius * cos( i ), radius * sin( i ) );
		nb.set( radius * cos( i + step ), radius * sin( i + step ) );
		drawLine( pos + na, pos + nb );
	}
}

void Renderer::drawTriangle( const Vector4& a, const Vector4& b, const Vector4& c, unsigned int color )
{
	m_displayTris.push_back( DisplayTriangle( a, b, c, color ) );
}

void Renderer::drawText( const std::string& str, const Vector4& pos, const Real scale, unsigned int color )
{
	m_displayTexts.push_back( DisplayText( str, pos, scale, color ) );
}
