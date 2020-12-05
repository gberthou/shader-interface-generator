#version 130

uniform float ratio;
uniform float d;
uniform float t;

float smin(float x, float y)
{
    const float k = log(2);
    float a = exp(-k * exp(5*(x - y)));
    return a * x + (1.0 - a) * y;
}

float sdfSphere(vec3 pos, vec3 center, float radius)
{
    return length(pos - center) - radius;
}

float sdfScene(vec3 pos)
{
    const vec3 sphereCenterA = vec3(0, 0, 2);
    const float sphereRadiusA = 1.f;
    vec3 sphereCenterB = vec3(2*exp(-t/30)*cos(t), 2*exp(-t/30)*sin(t), 2);
    const float sphereRadiusB = 0.25f;

    return smin(
        sdfSphere(pos, sphereCenterA, sphereRadiusA),
        sdfSphere(pos, sphereCenterB, sphereRadiusB)
    );
}

vec3 normal(vec3 pos)
{
    const float EPSILON = 1e-5;
    return normalize(vec3(
        sdfScene(vec3(pos.x + EPSILON, pos.yz)) - sdfScene(vec3(pos.x - EPSILON, pos.yz)),
        sdfScene(vec3(pos.x, pos.y + EPSILON, pos.z)) - sdfScene(vec3(pos.x, pos.y - EPSILON, pos.z)),
        sdfScene(vec3(pos.xy, pos.z + EPSILON)) - sdfScene(vec3(pos.xy, pos.z - EPSILON))
    ));
}

void main()
{
    const uint MAXITER = 100u;
    const float EPSILON = 1e-5;
    vec3 pos = vec3(0.0, 0.0, -d);
    vec3 u = normalize(vec3(gl_PointCoord.x * ratio, gl_PointCoord.y, d));

    float dist = sdfScene(pos);
    float depth = dist;
    for(uint i = 0u; i < MAXITER; ++i)
    {
        pos += dist * u;
        dist = sdfScene(pos);
        if(dist < EPSILON)
        {
            vec3 n = normal(pos);
            gl_FragColor = vec4(-n.z, -n.z, -n.z, 1.);
            return;
        }
    }
    gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
}
