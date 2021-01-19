#include <iostream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

#include <gl.h>
#include <GLFW/glfw3.h>
#include <utils.h>
#include <glmutils.h>

#include "shaders/ProgramWorld.h"

static void initGL(unsigned int width, unsigned int height)
{
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST); 
    glDepthFunc(GL_LEQUAL);
    glClearColor(0, 0, 1, 1);
}

int main(void)
{
    const unsigned int width = 1920;
    const unsigned int height = 1080;

    glfwInit();
    GLFWwindow *window = glfwCreateWindow(width, height, "Terrain", 0, 0);
    glfwMakeContextCurrent(window);

    initGL(width, height);

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

    double actualTime = glfwGetTime();
    size_t frames = 0;

    while(!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        programWorld.Apply();
        programWorld.Sett(t);
        programWorld.Bind();
        glDrawArrays(GL_PATCHES, 0, 6 * 64 * 1000);

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

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}

