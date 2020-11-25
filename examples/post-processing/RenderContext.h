#include <functional>

#include <framebuffer.h>

#include "shaders/ProgramWorld.h"
#include "shaders/ProgramScreen.h"

class RenderContext
{
    public:
        RenderContext(GLsizei width, GLsizei height, const std::string &code_world_vert, const std::string &code_world_frag, const std::string &code_screen_vert, const std::string &code_screen_frag);

        const ProgramWorld &GetProgram0() const;
        const ProgramScreen &GetProgram1() const;

        void Draw0(const std::function<void()> &draw) const;
        void Draw1(const std::function<void()> &draw) const;

    protected:
        ProgramWorld  programWorld;
        ProgramScreen programScreen;
        Framebuffer framebuffer;
};

