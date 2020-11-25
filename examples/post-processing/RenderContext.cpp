#include <gl.h>

#include "RenderContext.h"
#include "shaders/ProgramScreen.h"

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

RenderContext::RenderContext(GLsizei width, GLsizei height, const std::string &code_world_vert, const std::string &code_world_frag, const std::string &code_screen_vert, const std::string &code_screen_frag):
    programWorld(code_world_vert, code_world_frag),
    programScreen(code_screen_vert, code_screen_frag),
    framebuffer(width, height)
{
    initSquare(programScreen);
}

const ProgramWorld &RenderContext::GetProgram0() const
{
    return programWorld;
}

const ProgramScreen &RenderContext::GetProgram1() const
{
    return programScreen;
}

void RenderContext::Draw0(const std::function<void()> &draw) const
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, framebuffer.GetColorTexture());

    framebuffer.Bind();
    draw();
    framebuffer.Unbind();
}

void RenderContext::Draw1(const std::function<void()> &draw) const
{
    draw();
}

