#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

#include <gl.h>
#include <ui.h>
#include <glmutils.h>
#include <utils.h>

#include "shaders/ProgramWorld.h"

static void initGL(unsigned int width, unsigned int height)
{
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST); 
    glDepthFunc(GL_LEQUAL);
    glClearColor(1, 1, 1, 1);
}

static void initCube(ProgramWorld &program, std::vector<GLuint> &indices)
{
    const ProgramWorld::Vertex vertices[] = {
        // Top face
        {
            .aposition = {-1, 1, 1},
            .anormal = {0, 0, 1},
            .acolor = {1, 0, 0}
        },
        {
            .aposition = {1, 1, 1},
            .anormal = {0, 0, 1},
            .acolor = {1, 0, 0}
        },
        {
            .aposition = {1, -1, 1},
            .anormal = {0, 0, 1},
            .acolor = {1, 0, 0}
        },
        {
            .aposition = {-1, -1, 1},
            .anormal = {0, 0, 1},
            .acolor = {1, 0, 0}
        },

        // Bottom face
        {
            .aposition = {-1, 1, -1},
            .anormal = {0, 0, -1},
            .acolor = {1, 1, 0}
        },
        {
            .aposition = {1, 1, -1},
            .anormal = {0, 0, -1},
            .acolor = {1, 1, 0}
        },
        {
            .aposition = {1, -1, -1},
            .anormal = {0, 0, -1},
            .acolor = {1, 1, 0}
        },
        {
            .aposition = {-1, -1, -1},
            .anormal = {0, 0, -1},
            .acolor = {1, 1, 0}
        },

        // Front face
        {
            .aposition = {-1, 1, 1},
            .anormal = {0, 1, 0},
            .acolor = {0, 1, 0}
        },
        {
            .aposition = {1, 1, 1},
            .anormal = {0, 1, 0},
            .acolor = {0, 1, 0}
        },
        {
            .aposition = {1, 1, -1},
            .anormal = {0, 1, 0},
            .acolor = {0, 1, 0}
        },
        {
            .aposition = {-1, 1, -1},
            .anormal = {0, 1, 0},
            .acolor = {0, 1, 0}
        },
        
        // Back face
        {
            .aposition = {-1, -1, 1},
            .anormal = {0, -1, 0},
            .acolor = {0, 1, 1}
        },
        {
            .aposition = {1, -1, 1},
            .anormal = {0, -1, 0},
            .acolor = {0, 1, 1}
        },
        {
            .aposition = {1, -1, -1},
            .anormal = {0, -1, 0},
            .acolor = {0, 1, 1}
        },
        {
            .aposition = {-1, -1, -1},
            .anormal = {0, -1, 0},
            .acolor = {0, 1, 1}
        },

        // Left face
        {
            .aposition = {-1, -1, 1},
            .anormal = {-1, 0, 0},
            .acolor = {0, 0, 1}
        },
        {
            .aposition = {-1, 1, 1},
            .anormal = {-1, 0, 0},
            .acolor = {0, 0, 1}
        },
        {
            .aposition = {-1, 1, -1},
            .anormal = {-1, 0, 0},
            .acolor = {0, 0, 1}
        },
        {
            .aposition = {-1, -1, -1},
            .anormal = {-1, 0, 0},
            .acolor = {0, 0, 1}
        },

        // Right face
        {
            .aposition = {1, -1, 1},
            .anormal = {1, 0, 0},
            .acolor = {1, 0, 1}
        },
        {
            .aposition = {1, 1, 1},
            .anormal = {1, 0, 0},
            .acolor = {1, 0, 1}
        },
        {
            .aposition = {1, 1, -1},
            .anormal = {1, 0, 0},
            .acolor = {1, 0, 1}
        },
        {
            .aposition = {1, -1, -1},
            .anormal = {1, 0, 0},
            .acolor = {1, 0, 1}
        }
    };

    program.Bind();
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    indices = {
        0, 1, 2,
        0, 2, 3,

        4, 5, 6,
        4, 6, 7,

        8, 9, 10,
        8, 10, 11,

        12, 13, 14,
        12, 14, 15,

        16, 17, 18,
        16, 18, 19,

        20, 21, 22,
        20, 22, 23,
    };
}

int main(void)
{
    const unsigned int width = 800;
    const unsigned int height = 600;

    UI ui(width, height, "Rotating Light");

    initGL(width, height);

    ProgramWorld program(FileContents("shaders/World.vert"), FileContents("shaders/World.frag"));
    std::vector<GLuint> cubeIndices;
    initCube(program, cubeIndices);

    program.Apply();
    glm::mat4 proj = glm::perspective(100.f, static_cast<float>(width)/height, 1.f, 100.f);
    program.Setuproj(mat2array(proj));

    glm::mat4 view = glm::lookAt(
        glm::vec3(10, 10, 5),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 0, 1)
    );
    program.Setuview(mat2array(view));

    program.Setumodel(mat2array(glm::mat4(1.)));

    float theta = 0.f;

    while(ui.PollEvent())
    {
        program.Apply();
        program.Bind();

        program.Setulightpos({
            4 * cosf(theta),
            4 * sinf(theta),
            3 * cosf(4. * theta)
        });

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawElements(GL_TRIANGLES, cubeIndices.size(), GL_UNSIGNED_INT, cubeIndices.data());
        ui.Refresh();

        theta += 0.05f;
    }

    return EXIT_SUCCESS;
}

