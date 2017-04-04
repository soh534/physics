#version 120

attribute vec2 pos;
attribute vec4 col;
 
varying vec4 colFrag;

void main()
{
    gl_Position	= vec4(pos, 0.0, 1.0);
	colFrag = col;
}