#version 400

layout( location = 0 ) in vec4 position; /// (pos.x, pos.y, uv.x, uv.y)
layout( location = 1 ) in vec4 color;

out vec2 uvToFragmentShader;
out vec4 colorToFragmentShader;

void main()
{
	/// todo:
	/// replace this part with a projection matrix
	/// pass screen dimensions properly
	vec4 positionClipspace = position - vec4( 1024 / 2, 768 / 2, 0, 1 );
	gl_Position = positionClipspace / vec4( 1024 / 2, 768 / 2, 0, 1 );
	
	uvToFragmentShader = position.zw;
	colorToFragmentShader = color;
}
