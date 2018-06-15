#version 400

layout( location = 0 ) in vec3 position; // Local space
layout( location = 1 ) in vec4 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Lighting
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

out vec4 vertColor;

void main()
{
	gl_Position = projection * view	* model	* vec4( position, 1.f );
	vertColor = color;
}