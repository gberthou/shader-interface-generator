#version 440
layout(triangles) in;
layout(triangle_strip, max_vertices = 13) out;

in mat4 geo_projection[];

out vec3 frag_original_normal;
out vec3 frag_original_vertex;
out vec3 frag_normal;

out float frag_slope;
out float frag_fog_factor;
out float frag_canopy;
out vec4 frag_canopy_center;

float fog(vec4 position_world)
{
    float fog = position_world.z + sin(position_world.y) / 8.0;
    fog = (fog / 10.0) + 0.5 + 0.4;
    fog = fog * fog * fog;
    return exp(-fog);
}

void main()
{
    vec4 ab = gl_in[1].gl_Position - gl_in[0].gl_Position;
    vec4 ac = gl_in[2].gl_Position - gl_in[0].gl_Position;
    vec3 normal_world = normalize(cross(ab.xyz, ac.xyz));
    if(normal_world.z < 0.0)
        normal_world = -normal_world;
    vec3 normal = (geo_projection[0] * vec4(normal_world, 0.0)).xyz;

    float slope = abs(normal_world.z);
    slope *= slope;
    slope *= slope;
    slope *= slope;
    slope *= 0.8;
    slope = 1.0 - slope;

    for(uint i = 0u; i < 3u; ++i)
    {
        frag_original_normal = normal_world;
        frag_normal = normal;
        frag_original_vertex = gl_in[i].gl_Position.xyz;
        frag_slope = slope;
        frag_fog_factor = fog(gl_in[i].gl_Position);
        frag_canopy = -1.0;
        gl_Position = geo_projection[0] * gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();

    if(slope < 0.225)
    {
#if 0
        const float TREE_RADIUS = 0.1;
        const float TREE_HEIGHT = 1.5; 
        const float CANOPY_RADIUS = 1.0;
        const vec4 vertices[4] = vec4[4](
            vec4(-0.5, 0.0, 0.0, 1.0),
            vec4(-0.5, 0.0, 1.0, 1.0),
            vec4(0.5, 0.0, 0.0, 1.0),
            vec4(0.5, 0.0, 1.0, 1.0)
        );

        vec4 center = (gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position) / 3.0;
        vec4 proj_center = geo_projection[0] * center;

        for(uint i = 0u; i < 4u; ++i)
        {
            vec3 v = vertices[i].xyz;
            v.x *= TREE_RADIUS;
            v.z *= TREE_HEIGHT;
            v += center.xyz;

            frag_original_normal = normal_world;
            frag_normal = normal;
            frag_original_vertex = v.xyz;
            frag_slope = slope;
            frag_fog_factor = fog(vec4(v, 1.0));
            frag_canopy = -1.0;
            gl_Position = geo_projection[0] * vec4(v, 1.0);
            EmitVertex();
        }
        EndPrimitive();
#endif

#if 0
        const vec2 screen[4] = vec2[4](
            vec2(-1.0, -1.0),
            vec2(-1.0, 1.0),
            vec2(1.0, -1.0),
            vec2(1.0, 1.0)
        );

        for(uint i = 0u; i < 4u; ++i)
        {
            /*
            vec3 v = vertices[i].xyz + vec3(0.0, 0.0, TREE_HEIGHT * 0.8);
            v.x *= CANOPY_RADIUS;
            v.z *= CANOPY_RADIUS;
            v += center.xyz;
            */

            frag_original_normal = normal_world;
            frag_normal = normal;
            //frag_original_vertex = v.xyz;
            frag_slope = slope;
            //frag_fog_factor = fog(vec4(v, 1.0));
            frag_canopy = 1.0;
            frag_canopy_center = proj_center;
            //gl_Position = geo_projection[0] * vec4(v, 1.0);
            gl_Position = vec4(screen[i], proj_center.z, 1.0);
            EmitVertex();
        }
        EndPrimitive();
#endif
    }
}

