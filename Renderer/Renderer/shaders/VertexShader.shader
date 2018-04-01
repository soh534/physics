#version 400

//layout( location = 0 ) in vec3 position;
layout( location = 0 ) in vec2 position;
layout( location = 1 ) in vec4 color;

//uniform mat4 model;
//uniform mat4 view;
uniform mat4 projection;
 
out vec4 colorToFragmentShader;

void main()
{
	//gl_Position = projection * view * model * vec4( position, 1.f );
	gl_Position = projection * vec4( position, 0.f, 1.f );
	colorToFragmentShader = color;
}