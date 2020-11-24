#version 130

in vec2 aposition;

void main()
{
    gl_Position = vec4(aposition, 0., 1.);
}
