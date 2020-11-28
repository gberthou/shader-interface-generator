#include <glm/gtc/matrix_transform.hpp>

#include <gl.h>
#include <glmutils.h>
#include <ui.h>
#include <utils.h>
#include <framebuffer.h>

#include "shaders/ProgramDrawTechnique.h"
#include "cube.h"

static void initGL(unsigned int width, unsigned int height)
{
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST); 
    glDepthFunc(GL_LEQUAL);
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

static void drawScreen()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

int main(void)
{
    const unsigned int width = 800;
    const unsigned int height = 600;

    UI ui(width, height, "Hello World!");

    initGL(width, height);

    ProgramDrawTechnique renderer(
        width, height,
        width, height,
        FileContents("shaders/World.vert"), FileContents("shaders/World.frag"),
        FileContents("shaders/Screen.vert"), FileContents("shaders/Screen.frag")
    );
    const auto &programWorld  = renderer.GetProgram0();
    const auto &programScreen = renderer.GetProgram1();

    std::vector<GLuint> cubeIndices;
    CubeInit(programWorld, cubeIndices);

    initSquare(programScreen);

    // Set the uniform variables of the world program
    programWorld.Apply();
    glm::mat4 proj = glm::perspective(100.f, static_cast<float>(width)/height, 1.f, 100.f);
    programWorld.Setuproj(mat2array(proj));

    glm::mat4 view = glm::lookAt(
        glm::vec3(10, 10, 5),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 1)
    );
    programWorld.Setuview(mat2array(view));
 
    // Bind the framebuffers to TEXTURE0 and TEXTURE1
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderer.GetFramebuffer0().GetColorTexture());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, renderer.GetFramebuffer1().GetColorTexture());

    float t = 0.;

    while(ui.PollEvent())
    {
        // Update model matrix
        programWorld.Apply();
        programWorld.Setumodel(mat2array(glm::rotate(glm::mat4(1.), t, glm::vec3(.5, .5, .5))));
        t += .01;

        // First: Render World onto framebuffer
        renderer.Draw0([&]{
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDrawElements(GL_TRIANGLES, cubeIndices.size(), GL_UNSIGNED_INT, cubeIndices.data());
        });

        // Second: First pass (horizontal)
        programScreen.Apply();
        programScreen.Setscreen(0);
        programScreen.Sethorizontal(true);
        programScreen.Setnstep(width / 4);
        renderer.Draw1(drawScreen);

        // Third: Second pass (vertical)
        //programScreen.Apply();
        programScreen.Setscreen(1);
        programScreen.Sethorizontal(false);
        programScreen.Setnstep(height / 4);
        renderer.Draw2(drawScreen);

        ui.Refresh();
    }

    return EXIT_SUCCESS;
}

