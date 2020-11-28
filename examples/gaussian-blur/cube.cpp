#include "cube.h"

void CubeInit(const ProgramWorld &program, std::vector<GLuint> &indices)
{
    const ProgramWorld::Vertex vertices[] = {
        // Top face
        {
            .aposition = {-1, 1, 1},
            .acolor = {1, 0, 0}
        },
        {
            .aposition = {1, 1, 1},
            .acolor = {1, 0, 0}
        },
        {
            .aposition = {1, -1, 1},
            .acolor = {1, 0, 0}
        },
        {
            .aposition = {-1, -1, 1},
            .acolor = {1, 0, 0}
        },

        // Bottom face
        {
            .aposition = {-1, 1, -1},
            .acolor = {1, 1, 0}
        },
        {
            .aposition = {1, 1, -1},
            .acolor = {1, 1, 0}
        },
        {
            .aposition = {1, -1, -1},
            .acolor = {1, 1, 0}
        },
        {
            .aposition = {-1, -1, -1},
            .acolor = {1, 1, 0}
        },

        // Front face
        {
            .aposition = {-1, 1, 1},
            .acolor = {0, 1, 0}
        },
        {
            .aposition = {1, 1, 1},
            .acolor = {0, 1, 0}
        },
        {
            .aposition = {1, 1, -1},
            .acolor = {0, 1, 0}
        },
        {
            .aposition = {-1, 1, -1},
            .acolor = {0, 1, 0}
        },
        
        // Back face
        {
            .aposition = {-1, -1, 1},
            .acolor = {0, 1, 1}
        },
        {
            .aposition = {1, -1, 1},
            .acolor = {0, 1, 1}
        },
        {
            .aposition = {1, -1, -1},
            .acolor = {0, 1, 1}
        },
        {
            .aposition = {-1, -1, -1},
            .acolor = {0, 1, 1}
        },

        // Left face
        {
            .aposition = {-1, -1, 1},
            .acolor = {0, 0, 1}
        },
        {
            .aposition = {-1, 1, 1},
            .acolor = {0, 0, 1}
        },
        {
            .aposition = {-1, 1, -1},
            .acolor = {0, 0, 1}
        },
        {
            .aposition = {-1, -1, -1},
            .acolor = {0, 0, 1}
        },

        // Right face
        {
            .aposition = {1, -1, 1},
            .acolor = {1, 0, 1}
        },
        {
            .aposition = {1, 1, 1},
            .acolor = {1, 0, 1}
        },
        {
            .aposition = {1, 1, -1},
            .acolor = {1, 0, 1}
        },
        {
            .aposition = {1, -1, -1},
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

