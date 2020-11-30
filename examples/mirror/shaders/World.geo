#version 330

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in uint mustprune[];
in vec3 _color[];

out vec3 color;

void main()
{
    if(mustprune[0] == 0u || mustprune[1] == 0u ||mustprune[2] == 0u)
    {
        color = _color[0];
        gl_Position = gl_in[0].gl_Position;
        EmitVertex();
        gl_Position = gl_in[1].gl_Position;
        EmitVertex();
        gl_Position = gl_in[2].gl_Position;
        EmitVertex();
        EndPrimitive();
    }
}
