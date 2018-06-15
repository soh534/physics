#include "Shader.h"
#include <Common/Base.h>
#include <Common/FileIO.h>

#include <string>
#include <vector>

Shader::Shader( const char* vertShaderPath, const char* fragShaderPath )
{
	// TODO: constructors shouldn't fail. Add error handling
	std::string vertShaderSrcStr;
	Assert( FileIO::loadFileStream( vertShaderPath, vertShaderSrcStr ) == 0, "Failed to read shader" );

	std::string fragShaderSrcStr;
	Assert( FileIO::loadFileStream( fragShaderPath, fragShaderSrcStr ) == 0, "Failed to read shader" );

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile vertex shader
	printf( "Compiling vertex shader: %s\n", vertShaderPath );
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
	printf( "Compiling fragment shader: %s\n", fragShaderPath );
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
	m_id = glCreateProgram();
	glAttachShader( m_id, vertShaderId );
	glAttachShader( m_id, fragShaderId );
	glLinkProgram( m_id );

	glGetProgramiv( m_id, GL_LINK_STATUS, &Result );
	glGetProgramiv( m_id, GL_INFO_LOG_LENGTH, &InfoLogLength );
	if ( InfoLogLength > 0 )
	{
		std::vector<char> ProgramErrorMessage( InfoLogLength + 1 );
		glGetProgramInfoLog( m_id, InfoLogLength, nullptr, &ProgramErrorMessage[0] );
		printf( "%s\n", &ProgramErrorMessage[0] );
	}

	glDetachShader( m_id, vertShaderId );
	glDetachShader( m_id, fragShaderId );

	glDeleteShader( vertShaderId );
	glDeleteShader( fragShaderId );	
}
