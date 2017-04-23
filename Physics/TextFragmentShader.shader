#version 400

in vec2 uvToFragmentShader;
out vec4 fragmentColor;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{
	/// Only red-component is used by texture, use red-component as sampled alpha
	vec4 sampled = vec4( 1.0, 1.0, 1.0, texture( text, uvToFragmentShader ).r );

	/// Multiply sampled color to desired text color
	fragmentColor = vec4( textColor, 1.0 ) * sampled;
}