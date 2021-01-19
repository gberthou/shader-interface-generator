#version 440

void main()
{
    const vec2 _01 = vec2(0.0, 1.0);
    const vec2 vertices[6] = vec2[6](
        _01.xx,
        _01.yx,
        _01.yy,
        _01.xx,
        _01.yy,
        _01.xy
    );

    int index = (gl_VertexID % 6);
    int i = gl_VertexID / 6;

    float y = float(i >> 6) + vertices[index].y;
    float w = 200.0;
    float x = ((float(i & 0x3f) + vertices[index].x) / 64.0 - 0.5) * w;
    float length = w / 64.0;

    vec2 position = vec2(x, y);

    vec2 vertex = position;

    gl_Position = vec4(vertex, 0.0, 1.0);
}
