#version 130

in vec2 texcoord;

uniform sampler2D screen;

void main()
{
    gl_FragColor = texture(screen, vec2(texcoord.x, 1.0 - texcoord.y));
}
