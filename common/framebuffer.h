#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <GL/gl.h>

class Framebuffer
{
    public:
        Framebuffer(GLsizei width, GLsizei height, size_t colorAttachments, bool textureFiltering = true);
        ~Framebuffer();

        void Bind() const;
        static void Unbind();

        GLuint GetColorTexture(size_t index) const;
        GLuint GetDepthTexture() const;

    protected:
        size_t colorAttachments;
        GLuint fb;
        GLuint *textures;
};

#endif

