//#version 120

//varying vec2 uvFrag;

//uniform sampler2D texture;

//void main()
//{
//    gl_FragColor = texture2D(texture, uvFrag);
//}

#version 120

// Interpolated values from the vertex shaders
varying vec2 UV;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;

void main(){

	gl_FragColor = texture2D( myTextureSampler, UV );
	
	
}