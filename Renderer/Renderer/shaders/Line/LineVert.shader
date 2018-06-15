#version 400

layout( location = 0 ) in vec3 position; // In world space
layout( location = 1 ) in vec4 color;

uniform mat4 view;
uniform mat4 projection;

out vec4 vertColor;

void main()
{
	gl_Position = projection * view * vec4( position, 1.f );
	vertColor = color;
}