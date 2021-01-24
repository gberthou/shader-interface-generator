#version 440

uniform sampler3D canopy;

//in vec3 frag_original_normal;
//in vec3 frag_original_vertex;
flat in vec3 frag_normal;

flat in float frag_slope;
in float frag_fog_factor;
flat in float frag_canopy;
in vec3 frag_canopy_tex;

const vec4 SLOPE_COLOR    = vec4(0.6, 0.2, 0.1, 1.0);
const vec4 CLEARING_COLOR = vec4(0.1, 0.8, 0.4, 1.0);
const vec4 FOG_COLOR      = vec4(0.6, 0.8, 1.0, 1.0);
const vec4 LEAVES_COLOR   = vec4(0.1, 0.8, 0.2, 0.9);

void main()
{
    vec4 col;
    //vec4 canopyCol = texture(canopy, frag_canopy_tex);

    if(frag_canopy < 0.0)
    {
        // 1. Compute terrain color
        col = mix(CLEARING_COLOR, SLOPE_COLOR, frag_slope);
    }
    else
    {
        col = vec4(0.0, 1.0, 0.0, 1.0);
        //col = canopyCol;
        //if(col.a < 0.1)
        //    discard;
    }

    // 2. Compute color reflection
    col.xyz *= vec3(-frag_normal.z);

    // 3. Add fog based on height
    gl_FragColor = mix(col, FOG_COLOR, frag_fog_factor);
}

