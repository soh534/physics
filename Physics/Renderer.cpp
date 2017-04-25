#include <Base.hpp>

#include <Renderer.hpp>
#include <FileIO.hpp>

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <sstream>
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

std::vector<class Line> g_renderLines;
std::vector<class Text> g_renderTexts;

// For drawing lines
static GLuint g_lineProgramID = 0;
static GLuint g_lineVAO = 0;
static GLuint g_lineVBO[2] = { 0 };

// For drawing text
static GLuint g_txtProgramID = 0;
static GLuint g_txtVAO = 0;
static GLuint g_txtVBO = { 0 };

static int g_width = 0;
static int g_height = 0;

class Line
{
	
public:

	Line( const Vector3& pointA, const Vector3& pointB, unsigned int color );
	void render() const;

	Vector3 m_pointA, m_pointB;
	unsigned int m_color;
};

class Text
{

public:

	Text( const std::string& str, const Vector3& pos, unsigned int color );
	void render() const;

	std::string m_str;
	Vector3 m_pos;
	unsigned int m_color;
};

inline unsigned int SetRGBA( unsigned char r, unsigned char g, unsigned char b, unsigned char a )
{
	return ( r ) | ( g << 8 ) | ( b << 16 ) | ( a << 24 );
}

GLuint LoadShaders( const char* vertexShaderPath, const char* fragmentShaderPath )
{
	std::string vertShaderSrcStr;
	Assert( FileIO::loadFileStream( vertexShaderPath, vertShaderSrcStr ) == 0, "Failed to read shader" );

	std::string fragShaderSrcStr;
	Assert( FileIO::loadFileStream( fragmentShaderPath, fragShaderSrcStr ) == 0, "Failed to read shader" );

	GLint Result = GL_FALSE;
	int InfoLogLength;

	/// Compile vertex shader
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

	/// Compile fragment shader
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

	/// Link vertex and fragment shaders
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

struct Character
{
	GLuint m_textureId;
	Vector3 m_size;
	Vector3 m_bearing;
	GLuint m_advance;
};
std::map<GLchar, Character> Characters;

int initializeFreeType()
{
	FT_Library library;
	Assert( !FT_Init_FreeType( &library ), "failed to initialize freetype" );

	FT_Face face;
	Assert( !FT_New_Face( library, "C:\\Windows\\Fonts\\consola.ttf", 0, &face ),
			"failed to load font face" );

	//Assert( !FT_Set_Char_Size( face, 0, 16 * 64, g_width, g_height ),
	//		"failed to set font pixel size" );

	Assert( !FT_Set_Pixel_Sizes( face, 0, 48 ), "failed to define font size" );

	/// Pre-load characters
	/// Rendering technique taken from
	/// https://learnopengl.com/#!In-Practice/Text-Rendering

	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

	for ( GLubyte c = 0; c < 128; c++ )
	{
		Assert( !FT_Load_Char( face, c, FT_LOAD_RENDER ),
				"failed to load character from font face" );

		GLuint texture;
		glGenTextures( 1, &texture );
		glBindTexture( GL_TEXTURE_2D, texture );
		glTexImage2D( GL_TEXTURE_2D,
					  0,
					  GL_RED,
					  face->glyph->bitmap.width,
					  face->glyph->bitmap.rows,
					  0,
					  GL_RED,
					  GL_UNSIGNED_BYTE,
					  face->glyph->bitmap.buffer);

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		Character character =
		{
			texture,
			Vector3( face->glyph->bitmap.width, face->glyph->bitmap.rows ),
			Vector3( face->glyph->bitmap_left, face->glyph->bitmap_top ),
			face->glyph->advance.x
		};

		Characters.insert( std::pair<GLchar, Character>( c, character ) );
	}

	FT_Done_Face( face );
	FT_Done_FreeType( library );

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glGenVertexArrays( 1, &g_txtVAO );
	glGenBuffers( 1, &g_txtVBO );
	glBindVertexArray( g_txtVAO );
	glBindBuffer( GL_ARRAY_BUFFER, g_txtVBO );
	glBufferData( GL_ARRAY_BUFFER, sizeof( GLfloat ) * 6 * 4, nullptr, GL_DYNAMIC_DRAW );
	glEnableVertexAttribArray(0);
	glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof( GL_FLOAT ), 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindVertexArray( 0 );

	return 0;
}

int initializeRendering( int width, int height )
{
	GLenum err = glewInit();
	Assert( err == GLEW_OK, "failed to initialize glew" );

	/// This part isn't very design-obeying. Change it
	g_width = width;
	g_height = height;

#if defined FREETYPE_TEST
	g_lineProgramID = LoadShaders( "../Physics/VertexShader.shader", "../Physics/FragmentShader.shader" );
#else
	g_lineProgramID = LoadShaders( "VertexShader.shader", "FragmentShader.shader" );
#endif

	glGenVertexArrays( 1, &g_lineVAO );
	glGenBuffers( 2, g_lineVBO );
	glBindVertexArray( g_lineVAO );

	glBindBuffer( GL_ARRAY_BUFFER, g_lineVBO[0] );
	glBufferData( GL_ARRAY_BUFFER, sizeof( GL_FLOAT ) * 4, nullptr, GL_DYNAMIC_DRAW );
	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, nullptr );
	
	glBindBuffer( GL_ARRAY_BUFFER, g_lineVBO[1] );
	glBufferData( GL_ARRAY_BUFFER, sizeof( GL_FLOAT ) * 8, nullptr, GL_DYNAMIC_DRAW );
	glEnableVertexAttribArray( 1 );
	glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, 0, nullptr );

	glBindVertexArray( 0 );

#if defined FREETYPE_TEST
	g_txtProgramID = LoadShaders( "../Physics/TextVertexShader.shader", "../Physics/TextFragmentShader.shader" );
#else
	g_txtProgramID = LoadShaders( "TextVertexShader.shader", "TextFragmentShader.shader" );
#endif

	initializeFreeType();

	return 0;
}

int closeRendering()
{
	glDeleteProgram( g_lineProgramID );
	glDeleteProgram( g_txtProgramID );

	return 0;
}

int step()
{
	for ( int i = 0; i < (int)g_renderLines.size(); i++ )
	{
		g_renderLines[i].render();
	}

	g_renderLines.clear();

	for ( int i = 0; i < (int)g_renderTexts.size(); i++ )
	{
		g_renderTexts[i].render();
	}

	g_renderTexts.clear();

	return 0;
}

Line::Line( const Vector3& pointA, const Vector3& pointB, unsigned int color )
	: m_pointA(pointA), m_pointB(pointB), m_color(color)
{

}

void Line::render() const
{
	GLfloat vertices[] =
	{
		m_pointA( 0 ), m_pointA( 1 ), m_pointB( 0 ), m_pointB( 1 )
	};

	GLfloat color[] =
	{
		(GLfloat)(m_color & 0xff) / 255.f,
		(GLfloat)((m_color >> 8) & 0xff) / 255.f,
		(GLfloat)((m_color >> 16) & 0xff) / 255.f,
		(GLfloat)((m_color >> 24) & 0xff) / 255.f,
		(GLfloat)(m_color & 0xff) / 255.f,
		(GLfloat)((m_color >> 8) & 0xff) / 255.f,
		(GLfloat)((m_color >> 16) & 0xff) / 255.f,
		(GLfloat)((m_color >> 24) & 0xff) / 255.f
	};

	glUseProgram( g_lineProgramID );
	glBindVertexArray( g_lineVAO );
	glBindBuffer( GL_ARRAY_BUFFER, g_lineVBO[0] );
	glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( vertices ), vertices );
	glBindBuffer( GL_ARRAY_BUFFER, g_lineVBO[1] );
	glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( color ), color );
	glDrawArrays( GL_LINE_STRIP, 0, 2 );
	glBindVertexArray( 0 );
}

void addDisplay( const Line& line )
{
	g_renderLines.push_back( line );
}

void drawLine( const Vector3& pa, const Vector3& pb, unsigned int color )
{
	addDisplay( Line( pa, pb, color ) );
}

void drawCross( const Vector3& pos, const Real rot, const Real len, unsigned int color )
{
	Vector3 needle( len / 2.f, 0.f );

	Vector3 a, b, c, d;

	a = pos + needle.getRotatedDir( rot );
	b = pos + needle.getRotatedDir( rot + 90.f * g_degToRad );
	c = pos + needle.getRotatedDir( rot + 180.f * g_degToRad );
	d = pos + needle.getRotatedDir( rot + 270.f * g_degToRad );

	drawLine( a, c, color );
	drawLine( b, d, color );
}

void drawArrow( const Vector3& pos, const Vector3& dir, unsigned int color )
{
	Vector3 dst; dst.setAdd( pos, dir );
	drawLine( pos, dst, color );

	Vector3 dirFrac = dir * .5f;
	Vector3 headLeft = dirFrac.getRotatedDir( 135.f * g_degToRad );
	Vector3 headRight = dirFrac.getRotatedDir( -135.f * g_degToRad );
	Vector3 dstToHeadLeft = dst + headLeft;
	Vector3 dstToHeadRight = dst + headRight;

	drawLine( dst, dstToHeadLeft, color );
	drawLine( dst, dstToHeadRight, color );
}

void drawBox( const Vector3& max, const Vector3& min, unsigned int color )
{
	Vector3 upperLeft( min( 0 ), max( 1 ) );
	Vector3 bottomRight( max( 0 ), min( 1 ) );

	drawLine( max, upperLeft, color );
	drawLine( max, bottomRight, color );
	drawLine( min, upperLeft, color );
	drawLine( min, bottomRight, color );
}

void drawCircle( const Vector3& pos, const Real radius, unsigned int color )
{
	Real step = 2 * (Real)M_PI * STEP_RENDER_CIRCLE;

	for ( Real i = step; i < 2 * M_PI; i += step )
	{
		Vector3 na, nb;
		na.set( radius * cos( i ), radius * sin( i ) );
		nb.set( radius * cos( i + step ), radius * sin( i + step ) );
		drawLine( pos + na, pos + nb );
	}
}

#define RENDER_AXIS_MARKERS

void renderAxis()
{ // Draw full-length cross in origin
	drawLine( Vector3( -g_width / 2.f, 0.f ), Vector3( g_width / 2.f, 0.f ) );
	drawLine( Vector3( 0.f, g_height / 2.f ), Vector3( 0.f, -g_height / 2.f ) );

#if defined(RENDER_AXIS_MARKERS)
	int widthLimit = ( g_width / 20 ) * 10;
	int heightLimit = ( g_height / 20 ) * 10;

	const int interval = 50;

	for ( int i = 0; i > -widthLimit; i -= interval )
	{
		std::stringstream ss;
		ss << i;
		drawText( ss.str(), Vector3( (Real)i, 5.0f ) );
		drawLine( Vector3( (Real)i, 2.5f ), Vector3( (Real)i, -2.5f ) );
	}

	for ( int i = interval; i < widthLimit; i += interval )
	{
		std::stringstream ss;
		ss << i;
		drawText( ss.str(), Vector3( (Real)i, 5.0f ) );
		drawLine( Vector3( (Real)i, 2.5f ), Vector3( (Real)i, -2.5f ) );
	}

	for ( int i = 0; i > -heightLimit; i -= interval )
	{
		std::stringstream ss;
		ss << i;
		drawText( ss.str(), Vector3( 5.0f, (Real)i ) );
		drawLine( Vector3( 2.5f, (Real)i ), Vector3( -2.5f, (Real)i ) );
	}

	for ( int i = interval; i < heightLimit; i += interval )
	{
		std::stringstream ss;
		ss << i;
		drawText( ss.str(), Vector3( 5.0f, (Real)i ) );
		drawLine( Vector3( 2.5f, (Real)i ), Vector3( -2.5f, (Real)i ) );
	}
#endif
}

Text::Text( const std::string& str, const Vector3& pos, unsigned int color )
	: m_str( str ), m_pos( pos ), m_color( color )
{

}

void Text::render() const
{
	glUseProgram( g_txtProgramID );
	glUniform3f( glGetUniformLocation( g_txtProgramID, "textColor" ),
				 (GLfloat)(m_color & 0xff) / 255.f,
				 (GLfloat)(m_color >> 8 & 0xff) / 255.f,
				 (GLfloat)(m_color >> 16 & 0xff) / 255.f );

	// TODO: shouldn't need to do this every time
	glm::mat4 projection = glm::ortho( 0.f, 1024.f, 0.f, 768.f );
	glUniformMatrix4fv( glGetUniformLocation( g_txtProgramID, "projection" ), 
						1, 
						GL_FALSE, 
						glm::value_ptr( projection ) );

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(g_txtVAO);

	float scale = .3f;

	Real x = m_pos( 0 );
	Real y = m_pos( 1 );

    // Iterate through all characters
    std::string::const_iterator c;
    for (c = m_str.begin(); c != m_str.end(); c++)
	{
		Character ch = Characters[*c];

        GLfloat xpos = x + ch.m_bearing.x * scale;
        GLfloat ypos = y - (ch.m_size.y - ch.m_bearing.y) * scale;

        GLfloat w = ch.m_size.x * scale;
        GLfloat h = ch.m_size.y * scale;
        // Update VBO for each character
        GLfloat vertices[6][4] = {
            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos,     ypos,       0.0, 1.0 },
            { xpos + w, ypos,       1.0, 1.0 },

            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos + w, ypos,       1.0, 1.0 },
            { xpos + w, ypos + h,   1.0, 0.0 }
        };
        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.m_textureId);

        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, g_txtVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.m_advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void drawText( const std::string& str, const Vector3& pos, unsigned int color )
{
	g_renderTexts.push_back( Text( str, pos, color ) );
}
