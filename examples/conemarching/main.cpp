#include <gl.h>
#include <GLFW/glfw3.h>
#include <utils.h>

#include "shaders/ProgramWorld.h"

struct LayeredFramebuffer
{
    GLuint *fbs;
    GLuint *textures;
    size_t n;

    LayeredFramebuffer(size_t n):
        fbs(new GLuint[n]),
        textures(new GLuint[n]),
        n(n)
    {
    }

    LayeredFramebuffer(size_t width, size_t height, size_t layers):
        LayeredFramebuffer(layers - 1)
    {
        for(size_t i = 0; i + 1 < layers; ++i)
        {
            width  >>= 1;
            height >>= 1;

            glBindFramebuffer(GL_FRAMEBUFFER, fbs[i]);
            glBindTexture(GL_TEXTURE_2D, textures[i]);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            glBindTexture(GL_TEXTURE_2D, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textures[i], 0);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    ~LayeredFramebuffer()
    {
        glDeleteTextures(n, textures);
        glDeleteFramebuffers(n, fbs);

        delete [] fbs;
        delete [] textures;
    }
};

int main(void)
{
    const unsigned int width = 1920;
    const unsigned int height = 1080;
    const size_t layers = 3;

    glfwInit();
    GLFWwindow *window = glfwCreateWindow(width, height, "Conemarching", 0, 0);
    glfwMakeContextCurrent(window);

    LayeredFramebuffer fb(width, height, layers);

    glClearColor(0, 0, 0, 1);

    ProgramWorld program(FileContents("shaders/World.vert"), FileContents("shaders/World.frag"));

    float t = 0;

    program.Apply();
    program.Bind();
    program.Setd(1);
    program.SetcoarseDepth(0);

    glActiveTexture(GL_TEXTURE0);

    while(!glfwWindowShouldClose(window))
    {
        program.Sett(t);
        t += .05f;

        for(size_t layer = layers; layer--;)
        {
            unsigned int w = (width >> layer);
            unsigned int h = (height >> layer);

            glBindFramebuffer(GL_FRAMEBUFFER, layer == 0 ? 0 : fb.fbs[layer-1]);

            if(layer+1 != layers)
                glBindTexture(GL_TEXTURE_2D, fb.textures[layer]);
            glViewport(0, 0, w, h);
            program.Setdimensions({w, h});
            program.Setfirst(layer + 1 == layers);
            program.Setlast(layer == 0);

            glClear(GL_COLOR_BUFFER_BIT);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}

