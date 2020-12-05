#include <glm/gtc/matrix_transform.hpp>

#include <gl.h>
#include <GLFW/glfw3.h>
#include <glmutils.h>
#include <utils.h>
#include <framebuffer.h>

#include "shaders/ProgramWorld.h"
#include "cube.h"

#define NCUBES 4

const glm::vec3 MIRROR_DIMENSIONS(.1, 6, 8);

struct CubeDesc
{
    glm::vec3 center;
    float t;
};

static std::array<float, 3> vec2array(const glm::vec3 v)
{
    return {v.x, v.y, v.z};
}

static void initGL(unsigned int width, unsigned int height)
{
    glViewport(0, 0, width, height);

    glEnable(GL_DEPTH_TEST); 
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_STENCIL_TEST);

    glClearColor(1, 1, 1, 1);
}

static glm::mat4 modelMatrix(const CubeDesc &cube, const glm::vec3 &scale)
{
    return glm::scale(glm::rotate(glm::translate(glm::mat4(1.f), cube.center), cube.t, glm::vec3(0, 0, 1)), scale);
}

static void drawCube(const ProgramWorld &program, const CubeDesc &cube, const std::vector<GLuint> &cubeIndices, glm::vec3 scale = glm::vec3(1.f, 1.f, 1.f))
{
    program.Setumodel(mat2array(modelMatrix(cube, scale)));
    glDrawElements(GL_TRIANGLES, cubeIndices.size(), GL_UNSIGNED_INT, cubeIndices.data());
}

static void drawScene(const ProgramWorld &program, const std::array<CubeDesc, NCUBES> &cubes, const std::vector<GLuint> &cubeIndices)
{
    for(const auto &cube: cubes)
        drawCube(program, cube, cubeIndices);
}

static void DrawMirror(const ProgramWorld &program, const CubeDesc &cube, const std::vector<GLuint> &cubeIndices, bool mirrorface)
{
    if(!mirrorface)
    {
        std::vector<GLuint> partialIndices(cubeIndices.begin(), cubeIndices.end() - 6);
        drawCube(program, cube, partialIndices, MIRROR_DIMENSIONS);
    }
    else
    {
        std::vector<GLuint> partialIndices(cubeIndices.end() - 6, cubeIndices.end());
        drawCube(program, cube, partialIndices, MIRROR_DIMENSIONS);
    }
}

static void mirrorPointAndNormal(const CubeDesc &mirrorDesc, glm::vec3 &point, glm::vec3 &normal)
{
    const glm::vec4 pointOnMirrorLocal(1, 0, 0, 1);
    const glm::vec4 normalOnMirrorLocal(1, 0, 0, 0);
    const glm::mat4 model = modelMatrix(mirrorDesc, MIRROR_DIMENSIONS);
    point = model * pointOnMirrorLocal;
    normal = glm::normalize(model * normalOnMirrorLocal);
}

// All coordinates in world space
static glm::vec3 reflectPoint(const glm::vec3 &mirrorPoint, const glm::vec3 &mirrorNormal, const glm::vec3 &point)
{
    const glm::vec3 v = point - mirrorPoint;
    return point - (2 * glm::dot(v, mirrorNormal)) * mirrorNormal;
}

int main(void)
{
    const unsigned int width = 800;
    const unsigned int height = 600;

    glfwInit();
    GLFWwindow *window = glfwCreateWindow(width, height, "Stencil Mirror", 0, 0);
    glfwMakeContextCurrent(window);

    initGL(width, height);

    ProgramWorld programWorld(FileContents("shaders/World.vert"), FileContents("shaders/World.frag"), FileContents("shaders/World.geo"));

    std::vector<GLuint> cubeIndices;
    CubeInit(programWorld, cubeIndices);

    glm::vec3 eye(20, 10, 10);
    glm::vec3 target(0, 0, 0);
    const glm::vec3 vertical(0, 0, 1);

    const auto proj = mat2array(glm::perspective(static_cast<float>(M_PI) * 80.f / 180.f, static_cast<float>(width)/height, 1.f, 100.f));
    auto mirrorProj = proj;
    mirrorProj[0] = -mirrorProj[0];

    std::array<CubeDesc, NCUBES> cubes;
    for(size_t i = 0; i < cubes.size(); ++i)
    {
        cubes[i].center = glm::vec3(2 * i, 2 * i, 0);
        cubes[i].t = 10.f * i;
    }

    CubeDesc mirrorDesc = {
        .center = glm::vec3(0, 8, 3),
        .t = 0
    };

    programWorld.Apply();

    float t = 0;

    while(!glfwWindowShouldClose(window))
    {
        // Update eye and target
        float d = 4 * cos(.24 *t);
        eye.x = 20 + d;
        eye.y = 10 + d;
        target.x = d;
        target.y = d;
        t += .1f;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        programWorld.Setuview(mat2array(glm::lookAt(eye, target, vertical)));
        programWorld.Setuproj(proj);
        programWorld.Setualwayspass(true);

        // 1. Draw the un-mirrored scene (without the mirror surface but with the mirror frame) to populate color + depth buffer
        glStencilMask(0x00);
        drawScene(programWorld, cubes, cubeIndices);
        DrawMirror(programWorld, mirrorDesc, cubeIndices, false);

        // 2. Draw the mirror surface with color buffer disabled and update stencil only if depth buffer passes
        glStencilMask(0xff);
        glStencilFunc(GL_ALWAYS, 1, 0xff);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        DrawMirror(programWorld, mirrorDesc, cubeIndices, true);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        // 3. Re-draw scene (no mirror surface, no mirror frame, but the latter is optional) from the point of view of the reflected eye
        // Clear depth buffer to make a new rendering
        glClear(GL_DEPTH_BUFFER_BIT);
        glStencilMask(0x00);
        glStencilFunc(GL_EQUAL, 1, 0xff);

        glm::vec3 mirrorPoint;
        glm::vec3 mirrorNormal;
        mirrorPointAndNormal(mirrorDesc, mirrorPoint, mirrorNormal);

        glm::vec3 otherEye = reflectPoint(mirrorPoint, mirrorNormal, eye);
        glm::vec3 otherTarget = reflectPoint(mirrorPoint, mirrorNormal, target);

        programWorld.Setuview(mat2array(glm::lookAt(otherEye, otherTarget, vertical)));
        programWorld.Setuproj(mirrorProj);
        programWorld.Setualwayspass(false);
        programWorld.SetumirrorPoint(vec2array(mirrorPoint));
        programWorld.SetumirrorNormal(vec2array(mirrorNormal));

        drawScene(programWorld, cubes, cubeIndices);

        // Also, let's say that we are a cube so we can see our reflection
        CubeDesc player = {
            .center = eye,
            .t = M_PI / 2.f
        };
        drawCube(programWorld, player, cubeIndices);

        // Reset context
        glStencilMask(0xff);
        glStencilFunc(GL_ALWAYS, 1, 0xff);
        glEnable(GL_DEPTH_TEST);

        glfwSwapBuffers(window);
        glfwPollEvents();

        // Update cubes
        for(auto &cube : cubes)
            cube.t += .02f;
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}

