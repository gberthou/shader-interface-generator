#include <gl.h>
#include <ui.h>
#include <utils.h>
#include <framebuffer.h>

#include "shaders/ProgramWorld.h"
#include "shaders/ProgramScreen.h"
#include "RenderContext.h"

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
    RenderContext renderContext(
        width, height,
        FileContents("shaders/World.vert"),  FileContents("shaders/World.frag"),
        FileContents("shaders/Screen.vert"), FileContents("shaders/Screen.frag")
    );
    auto &programWorld = renderContext.GetProgram0();
    auto &programScreen = renderContext.GetProgram1();

    initTriangle(programWorld);

    // Set the uniform texture (named screen) to TEXTURE0
    programScreen.Bind();
    programScreen.Setscreen(0);

    while(ui.PollEvent())
    {
        // First: Render World onto the framebuffer
        renderContext.Draw0(
            [&programWorld]{drawWorld(programWorld);}
        );

        // Second: Render Screen onto the screen
        renderContext.Draw1(
            [&programScreen]{drawScreen(programScreen);}
        );

        ui.Refresh();
    }

    return EXIT_SUCCESS;
}

