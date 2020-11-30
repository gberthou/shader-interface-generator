#version 130

in vec3 aposition;
in vec3 acolor;

uniform mat4 umodel;
uniform mat4 uview;
uniform mat4 uproj;

uniform bool ualwayspass;
uniform vec3 umirrorPoint;
uniform vec3 umirrorNormal;

out vec3 _color;
out uint mustprune;

void main()
{
    vec4 pos = umodel * vec4(aposition, 1.0);

    _color = acolor;
    mustprune = (ualwayspass || dot(pos.xyz - umirrorPoint, umirrorNormal) > 0.0 ? 0u : 1u);

    gl_Position = uproj * uview * pos;
}
