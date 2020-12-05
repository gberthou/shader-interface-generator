#include <gl.h>
#include <GLFW/glfw3.h>
#include <utils.h>
#include <framebuffer.h>

#include "shaders/ProgramDrawTechnique.h"

static void initGL(unsigned int width, unsigned int height)
{
    glViewport(0, 0, width, height);
    glClearColor(1, 1, 1, 1);
}

static void initTriangle(const ProgramWorld &program)
{
    const ProgramWorld::Vertex vertices[] = {
        {.aposition = {-.75, -.75}},
        {.aposition = { .75, -.75}},
        {.aposition = {   0,  .75}},
    };

    program.Bind();
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

static void initSquare(const ProgramScreen &program)
{
    const ProgramScreen::Vertex vertices[] = {
        {
            .aposition = {-1, -1},
            .atexcoord = {0, 0}
        },
        {
            .aposition = {-1, 1},
            .atexcoord = {0, 1}
        },
        {
            .aposition = {1, -1},
            .atexcoord = {1, 0}
        },
        {
            .aposition = {1, 1},
            .atexcoord = {1, 1}
        }
    };

    program.Bind();
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

int main(void)
{
    const unsigned int width = 800;
    const unsigned int height = 600;

    glfwInit();
    GLFWwindow *window = glfwCreateWindow(width, height, "Post Processing", 0, 0);
    glfwMakeContextCurrent(window);

    initGL(width, height);
    ProgramDrawTechnique renderer(
        width, height,
        FileContents("shaders/World.vert"),  FileContents("shaders/World.frag"),
        FileContents("shaders/Screen.vert"), FileContents("shaders/Screen.frag")
    );
    auto &programWorld = renderer.GetProgram0();
    auto &programScreen = renderer.GetProgram1();

    initTriangle(programWorld);
    initSquare(programScreen);

    // Bind the intermediate framebuffer to TEXTURE0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderer.GetFramebuffer0().GetColorTexture());

    // Set the uniform texture (named screen) to TEXTURE0
    programScreen.Bind();
    programScreen.Setscreen(0);

    while(!glfwWindowShouldClose(window))
    {
        // First: Render World onto the framebuffer
        renderer.Draw0([]{
            glClear(GL_COLOR_BUFFER_BIT);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        });

        // Second: Render Screen onto the screen
        renderer.Draw1([]{
            glClear(GL_COLOR_BUFFER_BIT);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        });

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}

