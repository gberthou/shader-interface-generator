#include <iostream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

#include <gl.h>
#include <GLFW/glfw3.h>
#include <utils.h>
#include <glmutils.h>
#include <mathutils.h>
#include <framebuffer.h>

#include "shaders/ProgramWorld.h"
#include "shaders/ProgramCanopy.h"

/*
static GLuint initCanopy(void)
{
    const size_t width = 256;
    const size_t height = 256;
    const size_t depth = 256;

    GLuint fb;
    GLuint texture;

    glGenFramebuffers(1, &fb);
    glGenTextures(1, &texture);

    glBindFramebuffer(GL_FRAMEBUFFER, fb);
    glBindTexture(GL_TEXTURE_3D, texture);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, width, height, depth, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_3D, 0);

    glViewport(0, 0, width, height);
    glClearColor(0, 0, 0, 0);

    ProgramCanopy programCanopy(
        FileContents("shaders/Canopy.vert"),
        FileContents("shaders/Canopy.frag"),
        FileContents("shaders/Canopy.geo")
    );
    programCanopy.Apply();
    programCanopy.Bind();

    for(size_t layer = 0; layer < depth; ++layer)
    {
        glFramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_3D, texture, 0, layer);

        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glDeleteFramebuffers(1, &fb);
    return texture;
}
*/

int main(void)
{
    const unsigned int width = 1920;
    const unsigned int height = 1080;

    glfwInit();
    GLFWwindow *window = glfwCreateWindow(width, height, "Terrain", 0, 0);
    glfwMakeContextCurrent(window);

    //GLuint canopyTexture = initCanopy();

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST); 
    glDepthFunc(GL_LEQUAL);
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0, 0, 1, 1);

    ProgramWorld programWorld(
        FileContents("shaders/World.vert"),
        FileContents("shaders/World.frag"),
        FileContents("shaders/World.geo"),
        FileContents("shaders/World.tcs"),
        FileContents("shaders/World.tes")
    );

    float t = 0;

    programWorld.Apply();
    glm::mat4 proj = glm::perspective(static_cast<float>(M_PI) * 100.f / 180.f, static_cast<float>(width)/height, 0.5f, 100.f);
    programWorld.Setuproj(mat2array(proj));
    programWorld.Setcanopy(3);

    Framebuffer framebuffer(width, height, 1);
    //glActiveTexture(GL_TEXTURE3);
    //glBindTexture(GL_TEXTURE_3D, canopyTexture);

    double actualTime = glfwGetTime();
    size_t frames = 0;
    programWorld.Bind();

    while(!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        programWorld.Sett(t);
        glDrawArrays(GL_PATCHES, 0, 6 * 64 * 200);

        glfwSwapBuffers(window);
        glfwPollEvents();

        t += .25f;

        double now = glfwGetTime();
        double dt = now - actualTime;
        ++frames;
        if(dt >= 1.0)
        {
            std::cout << (frames / dt) << " fps" << std::endl;
            frames = 0;
            actualTime = now;
        }
    }

    //glDeleteTextures(1, &canopyTexture);

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}

