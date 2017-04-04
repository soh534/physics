//#version 120

//attribute vec2 pos;
//attribute vec2 uv;

//varying vec2 uvFrag;


//void main(){
//    vec2 posUnit = pos - vec2(512, 384);
//	posUnit /= vec2(512, 384);
//	gl_Position	= vec4(posUnit, 0, 1);
//
// 	uvFrag = uv;
//}

#version 120

// Input vertex data, different for all executions of this shader.
attribute vec2 vertexPosition_screenspace;
attribute vec2 vertexUV;

// Output data ; will be interpolated for each fragment.
varying vec2 UV;

void main(){

	// Output position of the vertex, in clip space
	// map [0..800][0..600] to [-1..1][-1..1]
	vec2 vertexPosition_homoneneousspace = vertexPosition_screenspace - vec2(400,300); // [0..800][0..600] -> [-400..400][-300..300]
	vertexPosition_homoneneousspace /= vec2(400,300);
	gl_Position =  vec4(vertexPosition_homoneneousspace,0,1);
	
	// UV of the vertex. No special space for this one.
	UV = vertexUV;
}

