#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>

class Shader
{
public:

	Shader( const char* vertShaderPath, const char* fragShaderPath );

	~Shader()
	{
		glDeleteProgram( m_id );
	}

	void use() const
	{
		glUseProgram( m_id );
	}

	void setVec3( const std::string& name, const float x, const float y, const float z ) const
	{
		glUniform3f( glGetUniformLocation( m_id, name.c_str() ), x, y, z );
	}

	void setMat4( const std::string& name, const glm::mat4& mat ) const
	{
		GLuint id = glGetUniformLocation( m_id, name.c_str() );
		glUniformMatrix4fv( id, 1, GL_FALSE, glm::value_ptr( mat ) );
	}

private:
	
	GLuint m_id;
};