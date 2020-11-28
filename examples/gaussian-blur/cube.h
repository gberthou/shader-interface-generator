#ifndef CUBE_H
#define CUBE_H

#include <vector>

#include <gl.h>

#include "shaders/ProgramDrawTechnique.h"

void CubeInit(const ProgramWorld &program, std::vector<GLuint> &indices);

#endif

