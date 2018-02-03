#version 400

in vec2 texCoords;

out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{
	// Only red-component is used by texture, use red-component as sampled alpha
	vec4 sampled = vec4( 1.f, 1.f, 1.f, texture( text, texCoords ).r );
	color = vec4( textColor, 1.f ) * sampled;
}