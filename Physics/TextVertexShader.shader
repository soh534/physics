#version 400

layout( location = 0 ) in vec4 position; /// (pos.x, pos.y, uv.x, uv.y)
out vec2 texCoords;

uniform mat4 projection;

void main()
{
	/// todo:
	/// replace this part with a projection matrix
	/// pass screen dimensions properly
	vec2 positionClipspace = position.xy - vec2(  1024 / 2, 768 / 2 );
	positionClipspace /= vec2(  1024 / 2, 768 / 2 );
	gl_Position = vec4( positionClipspace, 0, 1 );
	//gl_Position = projection * vec4(position.xy, 0.f, 1.f);
	
	texCoords = position.zw;
}
