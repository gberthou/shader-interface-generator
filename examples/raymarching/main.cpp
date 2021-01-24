#include <gl.h>
#include <GLFW/glfw3.h>
#include <utils.h>

#include "shaders/ProgramWorld.h"

static void initGL(unsigned int width, unsigned int height)
{
    glViewport(0, 0, width, height);
    glClearColor(1, 1, 1, 1);
}

static void initSquare(const ProgramWorld &program)
{
    const ProgramWorld::Vertex vertices[] = {
        {.aposition = {-1, -1}},
        {.aposition = {-1, 1}},
        {.aposition = {1, -1}},
        {.aposition = {1, 1}}
    };

    program.Bind();
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

int main(void)
{
    const unsigned int width = 1929;
    const unsigned int height = 1080;

    glfwInit();
    GLFWwindow *window = glfwCreateWindow(width, height, "Raymarching", 0, 0);
    glfwMakeContextCurrent(window);

    initGL(width, height);

    ProgramWorld program(FileContents("shaders/World.vert"), FileContents("shaders/World.frag"));
    initSquare(program);

    float t = 0;

    program.Apply();
    program.Setd(1);
    program.Setdimensions({width, height});

    while(!glfwWindowShouldClose(window))
    {
        program.Bind();
        program.Sett(t);
        t += .05f;

        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}

