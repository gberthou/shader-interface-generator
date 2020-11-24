#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <GL/gl.h>

class Framebuffer
{
    public:
        Framebuffer(GLsizei width, GLsizei height);
        ~Framebuffer();

        void Bind() const;
        static void Unbind();

        GLuint GetColorTexture() const;
        GLuint GetDepthTexture() const;

    protected:
        GLuint fb;
        GLuint textures[2];
};

#endif

