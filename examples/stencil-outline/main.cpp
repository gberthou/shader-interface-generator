#include <glm/gtc/matrix_transform.hpp>

#include <gl.h>
#include <GLFW/glfw3.h>
#include <glmutils.h>
#include <utils.h>
#include <framebuffer.h>

#include "shaders/ProgramWorld.h"
#include "shaders/ProgramStencil.h"
#include "cube.h"

#define NCUBES 4

struct CubeDesc
{
    glm::vec3 center;
    float t;
};

static void initGL(unsigned int width, unsigned int height)
{
    glViewport(0, 0, width, height);

    glEnable(GL_DEPTH_TEST); 
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);

    glClearColor(1, 1, 1, 1);
}

template<typename T>
static void drawCube(const T &program, const CubeDesc &cube, const std::vector<GLuint> &cubeIndices, float scale)
{
    glm::mat4 model = glm::scale(glm::rotate(glm::translate(glm::mat4(1.f), cube.center), cube.t, glm::vec3(.5, .4, .3)), glm::vec3(scale, scale, scale));
    program.Setumodel(mat2array(model));

    glDrawElements(GL_TRIANGLES, cubeIndices.size(), GL_UNSIGNED_INT, cubeIndices.data());
}

int main(void)
{
    const unsigned int width = 800;
    const unsigned int height = 600;

    glfwInit();
    GLFWwindow *window = glfwCreateWindow(width, height, "Stencil Outline", 0, 0);
    glfwMakeContextCurrent(window);

    initGL(width, height);

    ProgramWorld   programWorld(FileContents("shaders/World.vert"), FileContents("shaders/World.frag"));
    ProgramStencil programStencil(FileContents("shaders/World.vert"), FileContents("shaders/Stencil.frag"));

    std::vector<GLuint> cubeIndices;
    CubeInit(programWorld, cubeIndices);

    glm::mat4 view = glm::lookAt(
        glm::vec3(10, 6, 2),
        glm::vec3(0, NCUBES, 0),
        glm::vec3(0, 0, 1)
    );
    glm::mat4 proj = glm::perspective(static_cast<float>(M_PI) * 70.f / 180.f, static_cast<float>(width)/height, 1.f, 100.f);

    // Set the uniform variables of the world program
    programWorld.Apply();
    programWorld.Setuproj(mat2array(proj));
    programWorld.Setuview(mat2array(view));

    // Mirror those values in the stencil program
    programStencil.Apply();
    programStencil.Setuproj(mat2array(proj));
    programStencil.Setuview(mat2array(view));

    std::array<CubeDesc, NCUBES> cubes;
    for(size_t i = 0; i < cubes.size(); ++i)
    {
        cubes[i].center = glm::vec3(2 * i, 2 * i, 0);
        cubes[i].t = 10.f * i;
    }

    // Simple bitfield counter
    // bits  < 5: ignored
    // bits >= 5: 0 = unselected, 1 = selected
    unsigned int selected = 0;

    while(!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // Use usual world program
        programWorld.Apply();
        programWorld.Bind();

        // First, draw unselected cubes
        glStencilMask(0x00);
        for(size_t i = 0; i < cubes.size(); ++i)
            if(!(selected & (1 << (i + 5))))
                drawCube(programWorld, cubes[i], cubeIndices, 1.f);

        // Second, draw all cubes with stencil enabled
        // (stencil bytes will be set to 1 due to GL_REPLACE)
        glStencilFunc(GL_ALWAYS, 1, 0xff);
        glStencilMask(0xff);
        for(size_t i = 0; i < cubes.size(); ++i)
            if(selected & (1 << (i + 5)))
                drawCube(programWorld, cubes[i], cubeIndices, 1.f);

        // Use specific stencil program
        programStencil.Apply();
        programStencil.Bind();

        // Third, draw bigger versions of the selected cubes if the stencil
        // byte != 1
        glStencilFunc(GL_NOTEQUAL, 1, 0xff);
        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);
        for(size_t i = 0; i < cubes.size(); ++i)
            if(selected & (1 << (i + 5)))
                drawCube(programWorld, cubes[i], cubeIndices, 1.1f);

        // Reset context
        glStencilMask(0xff);
        glStencilFunc(GL_ALWAYS, 1, 0xff);
        glEnable(GL_DEPTH_TEST);

        glfwSwapBuffers(window);
        glfwPollEvents();

        // Update cubes
        for(auto &cube : cubes)
            cube.t += .02f;
        ++selected;
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}

