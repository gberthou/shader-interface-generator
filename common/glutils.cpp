#include <iostream>

#include "gl.h"

#include "glutils.h"

#define LOG_MAXLEN 1024
#define LOG_LEN(x) ((x) < LOG_MAXLEN ? (x) : LOG_MAXLEN)

namespace glutils
{
    Shader::Shader(GLenum type, const std::string &code):
        shader(glCreateShader(type))
    {
        if(shader == 0) // Error
        {
            // TODO
        }

        const GLchar *stringArray[1] = {code.c_str()};
        glShaderSource(shader, 1, stringArray, NULL);
        glCompileShader(shader);

        GLint success = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if(success == GL_FALSE)
        {
            GLsizei maxlength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxlength);
            GLsizei logsize = LOG_LEN(maxlength);
            GLsizei actuallength;

            char *s = new char[logsize];

            glGetShaderInfoLog(shader, logsize, &actuallength, s);
            std::cerr << "Error while compiling shader"
                      << std::endl << s << std::endl;
        }
    }

    Shader::~Shader()
    {
        glDeleteShader(shader);
    }

    void Shader::AttachTo(GLuint programID) const
    {
        glAttachShader(programID, shader);
    }


    Program::Program():
        program(glCreateProgram())
    {
    }

    Program::~Program()
    {
        glDeleteProgram(program);
    }

    void Program::EmplaceShader(GLenum type, const std::string &code)
    {
        shaders.emplace_back(type, code).AttachTo(program);
    }

    void Program::Finalize() const
    {
        glLinkProgram(program);
        glValidateProgram(program);
    }

    GLuint Program::Get() const
    {
        return program;
    }

    void Program::Apply() const
    {
        glUseProgram(program);
    }
}

