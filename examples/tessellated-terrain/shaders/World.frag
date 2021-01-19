#version 440
in vec3 frag_original_normal;
in vec3 frag_original_vertex;
in vec3 frag_normal;

in float frag_slope;
in float frag_fog_factor;
in float frag_canopy;
in vec4 frag_canopy_center;

const vec4 SLOPE_COLOR    = vec4(0.6, 0.2, 0.1, 1.0);
const vec4 CLEARING_COLOR = vec4(0.1, 0.8, 0.4, 1.0);
const vec4 FOG_COLOR      = vec4(0.6, 0.8, 1.0, 1.0);
const vec4 LEAVES_COLOR   = vec4(0.1, 0.8, 0.2, 0.9);

void main()
{
    vec4 color;

    if(frag_canopy < 0.0)
    {
        // 1. Compute terrain color
        color = mix(CLEARING_COLOR, SLOPE_COLOR, frag_slope);
    }
    else
    {
        vec2 a = gl_FragCoord.xy * 2.0 / vec2(1920.0, 1080.0) - 1.0;
        //vec2 a = gl_FragCoord.xy;
        float d = distance(a, frag_canopy_center.xy) / 2.0;
        float k = d;
        color = mix(LEAVES_COLOR, vec4(0.0, 0.0, 0.0, 0.0), k);
        if(color.a < 1e-1)
            discard;
    }

    // 2. Compute color reflection
    color.xyz *= vec3(-frag_normal.z);

    // 3. Add fog based on height
    color = mix(color, FOG_COLOR, frag_fog_factor);
    gl_FragColor = color;
}

