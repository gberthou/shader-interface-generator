import sys
import os
import json
from shaderdesc import *
from rendererdesc import to_renderer

# The format of sys.argv[1] should be json:
# {
#    "name"   : "name of the class/filenames to generate",
#    "shaders":
#    {
#        "vert": "vertex shader filename",
#        "frag": "fragment shader filename",
#        "geo" : "geometry shader filename",
#        "tcs" : "tessellation control shader filename",
#        "tes" : "tessellation evaluation shader filename"
#    }
# }

NAME = "name"
PROGRAMS = "programs"
SHADERS = "shaders"
RENDERORDER = "renderorder"

def types_of(program):
    return list({STYPE_VERT, STYPE_FRAG, STYPE_GEO, STYPE_TCS, STYPE_TES} & program.keys())

def generate_interface(program):
    types = types_of(program[SHADERS])
    return to_shader(program[NAME], program[SHADERS], types)

def renderer_name(name):
    return "Program" + name

for filename in sys.argv[1:]:
    with open(filename, "r") as f:
        os.chdir("shaders")
        data = json.load(f)

        if not RENDERORDER in data.keys():
            render_order = list(i[NAME] for i in data[PROGRAMS])
        else:
            render_order = list(data[RENDERORDER])

        if PROGRAMS in data.keys(): # Drawing technique composed of several passes
            shaders = list(generate_interface(program) for program in data[PROGRAMS])
            renderer = to_renderer(data[NAME], shaders, render_order)

            with open(renderer.header_name(), "w") as f:
                define = renderer.module_name().upper() + "_H"
                includes = ["string", "array", "functional", "glutils.h", "framebuffer.h"]

                f.write("#ifndef " + define + "\n#define " + define + "\n")
                f.write("\n".join("#include <" + i + ">" for i in includes) + "\n")
                f.write("\n".join(s.C_header_body() for s in shaders))
                f.write("\n")
                f.write(renderer.C_header_body())
                f.write("\n#endif\n")
            with open(renderer.source_name(), "w") as f:
                f.write("#include <gl.h>\n")
                f.write("#include \"" + renderer.header_name() + "\"\n")
                f.write("\n".join(s.C_source_body() for s in shaders))
                f.write("\n")
                f.write(renderer.C_source_body())

        else: # Single-pass program
            shader = generate_interface(data)

            with open(shader.header_name(), "w") as f:
                f.write(shader.C_header())
            with open(shader.source_name(), "w") as f:
                f.write(shader.C_source())

        os.chdir("..")
