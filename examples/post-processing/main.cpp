#include <gl.h>
#include <ui.h>
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

    UI ui(width, height, "Hello World!");

    initGL(width, height);
    ProgramDrawTechnique renderContext(
        width, height,
        FileContents("shaders/World.vert"),  FileContents("shaders/World.frag"),
        FileContents("shaders/Screen.vert"), FileContents("shaders/Screen.frag")
    );
    auto &programWorld = renderContext.GetProgram0();
    auto &programScreen = renderContext.GetProgram1();

    initTriangle(programWorld);
    initSquare(programScreen);

    // Bind the intermediate framebuffer to TEXTURE0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderContext.GetFramebuffer0().GetColorTexture());

    // Set the uniform texture (named screen) to TEXTURE0
    programScreen.Bind();
    programScreen.Setscreen(0);

    while(ui.PollEvent())
    {
        // First: Render World onto the framebuffer
        renderContext.Draw0([]{
            glClear(GL_COLOR_BUFFER_BIT);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        });

        // Second: Render Screen onto the screen
        renderContext.Draw1([]{
            glClear(GL_COLOR_BUFFER_BIT);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        });

        ui.Refresh();
    }

    return EXIT_SUCCESS;
}

