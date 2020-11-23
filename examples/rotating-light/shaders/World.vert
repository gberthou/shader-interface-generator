#version 130

in vec3 aposition;
in vec3 anormal;
in vec3 acolor;

uniform mat4 umodel;
uniform mat4 uview;
uniform mat4 uproj;
uniform vec3 ulightpos;

out vec3 color;

void main()
{
    vec4 pos = umodel * vec4(aposition, 1.0);

    vec3 lightdir = normalize(ulightpos - pos.xyz);
    float lightintensity = max(dot(anormal, lightdir), 0.0);
    color = lightintensity * acolor;
    

    gl_Position = uproj * uview * pos;
}
