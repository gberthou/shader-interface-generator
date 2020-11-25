#version 130

in vec3 aposition;
in vec3 acolor;

uniform mat4 umodel;
uniform mat4 uview;
uniform mat4 uproj;

out vec3 color;

void main()
{
    color = acolor;

    vec4 pos = umodel * vec4(aposition, 1.0);
    gl_Position = uproj * uview * pos;
}
