#ifndef GLUTILS_H
#define GLUTILS_H

#include <string>
#include <vector>

#include <GL/gl.h>

namespace glutils
{
    class Shader
    {
        public:
            Shader(GLenum type, const std::string &code);
            ~Shader();

            void AttachTo(GLuint programID) const;

        private:
            GLuint shader;
    };

    class Program
    {
        public:
            Program();
            ~Program();

            void EmplaceShader(GLenum type, const std::string &code);
            void Finalize() const;
            GLuint Get() const;

            virtual void Apply() const;

        protected:
            GLuint program;
            std::vector<Shader> shaders;
    };
}

#endif

