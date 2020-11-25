#version 130

in vec2 aposition;
in vec2 atexcoord;

out vec2 texcoord;

void main()
{
    texcoord = atexcoord;
    gl_Position = vec4(aposition, 0., 1.);
}
