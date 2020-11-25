#version 130

in vec2 texcoord;

uniform sampler2D screen;
uniform bool horizontal;
uniform uint nstep;

void main()
{
    vec3 sumColor = vec3(0.0, 0.0, 0.0);
    float sumWeight = 0.0;

    float step = 1.0 / nstep;

    for(float i = 0.0; i < 1.0; i += step)
    {
        float weight;
        vec2 fetchPos;

        if(horizontal)
        {
            weight = i - texcoord.x;
            fetchPos = vec2(i, texcoord.y);
        }
        else
        {
            weight = i - texcoord.y;
            fetchPos = vec2(texcoord.x, i);
        }
        weight = exp(-500 * weight * weight);

        sumColor += weight * texture(screen, fetchPos).xyz;
        sumWeight += weight;
    }

    gl_FragColor = vec4(sumColor / sumWeight, 1.0);
}
