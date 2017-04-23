#version 400

in vec2 uvToFragmentShader;
in vec4 colorToFragmentShader;

out vec4 fragmentColor;

uniform sampler2D text;

void main()
{
	/// Multiply sampled color to desired text color
	float alpha = texture(text, uvToFragmentShader).r;
	fragmentColor = vec4( colorToFragmentShader.rgb , alpha );
}