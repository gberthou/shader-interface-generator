#include <cmath>
#include <gl.h>
#include <ui.h>
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
    const unsigned int width = 800;
    const unsigned int height = 600;

    UI ui(width, height, "Hello World!");

    initGL(width, height);

    ProgramWorld program(FileContents("shaders/World.vert"), FileContents("shaders/World.frag"));
    initSquare(program);

    float t = 0;

    program.Apply();
    program.Setd(1);
    program.Setratio(static_cast<float>(width) / height);

    while(ui.PollEvent())
    {
        program.Bind();
        program.Sett(t);
        t += .05f;

        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        ui.Refresh();
    }

    return EXIT_SUCCESS;
}
