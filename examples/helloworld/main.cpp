#include <gl.h>
#include <ui.h>
#include <utils.h>

#include "shaders/ProgramWorld.h"

static void initGL(unsigned int width, unsigned int height)
{
    glViewport(0, 0, width, height);
    glClearColor(1, 1, 1, 1);
}

static void initTriangle(ProgramWorld &program)
{
    const ProgramWorld::Vertex vertices[] = {
        {.aposition = {-.75, -.75}},
        {.aposition = { .75, -.75}},
        {.aposition = {   0,  .75}},
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

    ProgramWorld program(FileContents("shaders/World.vert"), FileContents("shaders/World.frag"));
    initTriangle(program);

    program.Apply();

    while(ui.PollEvent())
    {
        program.Apply();
        program.Bind();

        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        ui.Refresh();
    }

    return EXIT_SUCCESS;
}

