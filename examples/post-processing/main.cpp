#include <gl.h>
#include <ui.h>
#include <utils.h>
#include <framebuffer.h>

#include "shaders/ProgramWorld.h"
#include "shaders/ProgramScreen.h"

static void initGL(unsigned int width, unsigned int height)
{
    glViewport(0, 0, width, height);
    glClearColor(1, 1, 1, 1);
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

static void drawWorld(const ProgramWorld &program)
{
    program.Apply();
    program.Bind();

    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

static void drawScreen(const ProgramScreen &program)
{
    program.Apply();
    program.Bind();

    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

int main(void)
{
    const unsigned int width = 800;
    const unsigned int height = 600;

    UI ui(width, height, "Hello World!");

    initGL(width, height);
    Framebuffer framebuffer(width, height);

    ProgramWorld  programWorld (FileContents("shaders/World.vert"),  FileContents("shaders/World.frag"));
    ProgramScreen programScreen(FileContents("shaders/Screen.vert"), FileContents("shaders/Screen.frag"));

    initTriangle(programWorld);
    initSquare(programScreen);

    // Bind the framebuffer to TEXTURE0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, framebuffer.GetColorTexture());

    // Set the uniform texture (named screen) to TEXTURE0
    programScreen.Bind();
    programScreen.Setscreen(0);

    while(ui.PollEvent())
    {
        // First: Render World onto framebuffer
        framebuffer.Bind();
        drawWorld(programWorld);
        framebuffer.Unbind();

        // Second: Render Screen onto the screen
        drawScreen(programScreen);

        ui.Refresh();
    }

    return EXIT_SUCCESS;
}

