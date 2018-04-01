#version 400

layout( location = 0 ) in vec4 position; // (pos.x, pos.y, uv.x, uv.y)
uniform mat4 projection;

out vec2 texCoords;

void main()
{
	gl_Position = projection * vec4(position.xy, 0.f, 1.f);
	texCoords = position.zw;
}
