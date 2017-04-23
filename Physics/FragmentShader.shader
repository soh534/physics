#version 400

in vec4 colorToFragmentShader;
out vec4 fragmentColor;

void main()
{
    fragmentColor = colorToFragmentShader;
}
