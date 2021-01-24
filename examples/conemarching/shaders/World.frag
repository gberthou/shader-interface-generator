#version 130

uniform sampler2D coarseDepth;
uniform bool first;
uniform bool last;

uniform float d;
uniform float t;
uniform uvec2 dimensions;

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

vec3 normal(vec3 pos, float dist)
{
    const vec2 EPSILON = vec2(1e-5, 0.0);
    return normalize(vec3(
        sdfScene(pos + EPSILON.xyy),
        sdfScene(pos + EPSILON.yxy),
        sdfScene(pos + EPSILON.yyx)
    ) - dist);
}

void main()
{
    const float MAXDIST = 5.0;
    const uint MAXITER = 100u;

    vec3 pos = vec3(0.0, 0.0, -d);
    vec2 screen = 2.0 * gl_FragCoord.xy / dimensions.y - vec2(float(dimensions.x) / dimensions.y, 1.0);
    vec3 u = normalize(vec3(screen, d));
    float accumulated = 0.0;

    if(!first)
    {
        vec2 p = gl_FragCoord.xy / dimensions;
        float dist = texture(coarseDepth, p).r * MAXDIST;
        accumulated += dist;
        pos += dist * u;
    }

    float r0 = 1.0 / dimensions.x / d;
    for(uint i = 0u; i < MAXITER && accumulated < MAXDIST; ++i)
    {
        float r = r0 * accumulated;
        
        float dist = sdfScene(pos);
        if(dist < r)
        {
            if(last)
            {
                vec3 n = normal(pos, dist);
                gl_FragColor = vec4(-n.z, -n.z, -n.z, 1.);
            }
            else
                gl_FragColor = vec4(accumulated / MAXDIST, 0.0, 0.0, 1.0);
            return;
        }
        pos += dist * u;
        accumulated += dist;
    }
    if(last)
        gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    else
        gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
