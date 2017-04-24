#version 400

in vec2 uvToFragmentShader;
in vec4 colorToFragmentShader;

out vec4 fragmentColor;

uniform sampler2D text;

void main()
{
	/// Only red-component is used by texture, use red-component as sampled alpha
	vec4 sampled = vec4( 1.0, 1.0, 1.0, texture( text, uvToFragmentShader ).r );
}