#include <glm/gtc/matrix_transform.hpp>

#include <gl.h>
#include <GLFW/glfw3.h>
#include <glmutils.h>
#include <utils.h>
#include <framebuffer.h>

#include "shaders/ProgramWorld.h"
#include "cube.h"

static void initGL(unsigned int width, unsigned int height)
{
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST); 
    glDepthFunc(GL_LEQUAL);
    glClearColor(1, 1, 1, 1);
}

int main(void)
{
    const unsigned int width = 800;
    const unsigned int height = 600;
    const int ntheta = 128;
    const int nphi = 128;

    glfwInit();
    GLFWwindow *window = glfwCreateWindow(width, height, "Instancing", 0, 0);
    glfwMakeContextCurrent(window);

    initGL(width, height);

    ProgramWorld programWorld(FileContents("shaders/World.vert"), FileContents("shaders/World.frag"));

    std::vector<GLuint> cubeIndices;
    CubeInit(programWorld, cubeIndices);

    // Set the uniform variables of the world program
    programWorld.Apply();
    glm::mat4 proj = glm::perspective(70.f * static_cast<float>(M_PI) / 180.f, static_cast<float>(width)/height, 1.f, 100.f);
    programWorld.Setuproj(mat2array(proj));

    glm::mat4 view = glm::lookAt(
        glm::vec3(3, 3, 0),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 1)
    );
    programWorld.Setuview(mat2array(view));

    programWorld.Setuntheta(ntheta);
    programWorld.Setunphi(nphi);

    float t = 0.;

    while(!glfwWindowShouldClose(window))
    {
        // Update model matrix
        programWorld.Apply();
        programWorld.Setut(t);
        t += .01;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawElementsInstanced(GL_TRIANGLES, cubeIndices.size(), GL_UNSIGNED_INT, cubeIndices.data(), ntheta * nphi);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}

