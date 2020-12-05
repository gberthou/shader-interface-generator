#version 140

in vec3 aposition;
in vec3 acolor;

uniform mat4 uview;
uniform mat4 uproj;
uniform int untheta;
uniform int unphi;
uniform float ut;

out vec3 color;

mat4 spherical(float theta, float phi)
{
    float ctheta = cos(theta);
    float stheta = sin(theta);
    float cphi   = cos(phi);
    float sphi   = sin(phi);

    return mat4(
        vec4(ctheta*cphi, ctheta*sphi, -stheta, 0.0),
        vec4(sphi, -cphi, 0.0, 0.0),
        vec4(stheta*cphi, stheta*sphi, ctheta, 0.0),
        vec4(0.0, 0.0, 0.0, 1.0)
    );
}

mat4 translate(vec3 pos)
{
    return mat4(
        vec4(1.0, 0.0, 0.0, 0.0),
        vec4(0.0, 1.0, 0.0, 0.0),
        vec4(0.0, 0.0, 1.0, 0.0),
        vec4(pos, 1.0)
    );
}

mat4 scale(float s)
{
    mat4 ret = mat4(s);
    ret[3][3] = 1.0;
    return ret;
}

void main()
{
    const float M_PI = 3.1415926535897932384626433832795; 

    int itheta  = gl_InstanceID % untheta;
    int iphi    = gl_InstanceID / untheta;
    float theta = itheta * M_PI / untheta;
    float phi   = iphi * 2.0 * M_PI / unphi;

    float localtheta = ut * gl_InstanceID / 200.0;
    float localphi = ut * gl_InstanceID / 200.0;

    mat4 model  = spherical(theta, phi) * translate(vec3(0, 0, 2)) * scale(2.0 / unphi) * spherical(localtheta, localphi);

    color = acolor;

    vec4 pos = model * vec4(aposition, 1.0);
    gl_Position = uproj * uview * pos;
}
