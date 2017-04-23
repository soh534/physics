#version 400

layout (location = 0) in vec2 position;
layout (location = 1) in vec4 color;
 
out vec4 colorToFragmentShader;

void main()
{
	/// todo: replace this part with a projection matrix
	vec2 positionClipspace = position - vec2( 1024 / 2, 768 / 2 );
	positionClipspace /= vec2( 1024 / 2, 768 / 2 );
    gl_Position	= vec4(positionClipspace, 0, 1);
	colorToFragmentShader = color;
}