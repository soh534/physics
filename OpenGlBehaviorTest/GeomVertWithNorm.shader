#version 400

layout( location = 0 ) in vec3 position; // Local space
layout( location = 1 ) in vec3 normal;
layout( location = 2 ) in vec4 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fragPos;
out vec3 normVector;
out vec4 vertColor;

void main()
{
    gl_Position = projection * view	* model	* vec4( position, 1.f );
    fragPos = vec3( model * vec4( position, 1.f ) );
    normVector = mat3( model ) * normal; // Take just rotation (3x3)
    vertColor = color;
}
