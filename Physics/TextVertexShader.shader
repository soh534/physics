#version 400

layout( location = 0 ) in vec4 position; /// (pos.x, pos.y, uv.x, uv.y)
out vec2 uvToFragmentShader;

void main()
{
	/// todo: replace this part with a projection matrix
	vec2 positionClipspace = position.xy - vec2(  1024 / 2, 768 / 2 );
	positionClipspace /= vec2(  1024 / 2, 768 / 2 );
	gl_Position = vec4( positionClipspace, 0, 1 );
	
	uvToFragmentShader = position.zw;
}
