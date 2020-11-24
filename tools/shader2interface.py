import sys
import re

import extractglsl

typevec     = re.compile(r"([biud])?vec([234])")
typemat     = re.compile(r"(d)?mat([234])x?([234])?")
typesampler = re.compile(r"([iu])?sampler(.+)")

STYPE_VERT = "vert"
STYPE_FRAG = "frag"
STYPE_GEO  = "geo"
STYPE_TCS  = "tcs"
STYPE_TES  = "tes"
SHADER_TYPES = ["vert", "frag", "geo", "tcs", "tes"]
STYPE_TO_GL = {
    STYPE_VERT: "GL_VERTEX_SHADER",
    STYPE_FRAG: "GL_FRAGMENT_SHADER",
    STYPE_GEO : "GL_GEOMETRY_SHADER",
    STYPE_TCS : "GL_TESS_CONTROL_SHADER",
    STYPE_TES : "GL_TESS_EVALUATIOB_SHADER"
}

def indent(s, n):
    lines = s.split("\n")
    return "\n".join(("    " * n) + line for line in lines)

def matdims(mmat):
    x = int(mmat.group(2))
    if mmat.group(3):
        return (x, int(mmat.group(3)))
    return (x, x)

def matdim(mmat):
    x, y = matdims(mmat)
    return x * y

def is_scalar(_type):
    # Todo: arrays?
    mvec     = typevec.match(_type)
    mmat     = typemat.match(_type)

    return not (mvec or mmat)

def Ctype_of(_type):
    if _type == "uint" or _type == "bool":
        return "GLuint"
    if _type == "GLint":
        return "int32_t"
    if _type == "double" or _type == "float":
        return "GLfloat"

    mvec     = typevec.match(_type)
    mmat     = typemat.match(_type)
    msampler = typesampler.match(_type)

    if mvec:
        __type = mvec.group(1)
        if __type == "b":
            subtype = "GLboolean"
        elif __type == "i":
            subtype = "GLint"
        elif __type == "u":
            subtype = "GLuint"
        else: # What of "d"?
            subtype = "GLfloat"
        return "std::array<" + subtype + ", " + mvec.group(2) + ">"
    if mmat:
        if mmat.group(1):
            subtype = "double"
        else:
            subtype = "float"

        dim = matdim(mmat)

        return "std::array<" + subtype + ", " + str(dim) + ">"
    if msampler:
        return "GLint"

    raise Exception("Ctype_of: unsupported type \"%s\"" % _type)

def uniform_type_of(_type):
    mvec = typevec.match(_type)
    mmat = typemat.match(_type)
    msampler = typesampler.match(_type)

    if _type == "uint" or _type == "bool":
        return "1uiv"
    if _type == "int":
        return "1iv"
    if _type == "float" or _type == "double":
        return "1fv"
    if mvec:
        __type = mvec.group(1)
        n = mvec.group(2)
        if __type == "b" or __type == "u":
            return mvec.group(2) + "uiv"
        if __type == "i":
            return mvec.group(2) + "iv"
        else: # What of "d"?
            return mvec.group(2) + "fv"
    elif mmat:
        x, y = matdims(mmat)
        if x != y:
            return "Matrix" + str(x) + "x" + str(y) + "fv"
        return "Matrix" + str(x) + "fv"
    elif msampler:
        if msampler.group(1) == None:
            return "1i"

def type_arity(_type):
    mvec = typevec.match(_type)
    if mvec:
        return int(mvec.group(2))

def default_vertex_type(_type):
    mvec = typevec.match(_type)
    if mvec:
        __type = mvec.group(1)
        if __type == None:
            return "GL_FLOAT"


def C_member(couple):
    return "GLint " + couple[0] + ";"

def C_setter_prototype(couple, prefix = ""):
    name, _type = couple
    ctype = Ctype_of(_type)
    return "void " + prefix + "Set" + name + " (const " + ctype + " &_" + name + ") const"

def C_bind_prototype(prefix = ""):
    return "void " + prefix + "Bind() const"


def C_code_arg(shadertype):
    return "code" + shadertype

def C_assign_attribute(couple):
    return couple[0] + " = glGetAttribLocation(program, \"" + couple[0] + "\");"

def C_assign_uniform(couple):
    return couple[0] + " = glGetUniformLocation(program, \"" + couple[0] + "\");"

def C_default_attrib_member(couple):
    name, _type = couple
    mvec = typevec.match(_type)
    mmat = typemat.match(_type)

    if mvec:
        __type = mvec.group(1)
        if __type == None:
            return "GLfloat " + name + "[" + mvec.group(2) + "];"
    elif mmat:
        pass

def C_VertexAttribPointer(classname, attributes):
    tmp = ""
    for name, _type in attributes.items():
        #stride = "sizeof(" + classname + "::Vertex) - sizeof(((" + classname + "::Vertex*)0)->" + name + ")"
        stride = "sizeof(" + classname + "::Vertex)"
        offset = "(void*) offsetof(" + classname + "::Vertex, " + name + ")"
        tmp += "glVertexAttribPointer(" + name + ", " + str(type_arity(_type)) + ", " + default_vertex_type(_type) + ", GL_FALSE, " + stride + ", " + offset + ");\n"
    return tmp

class Shader:
    def __init__(self, name, shadertypes, attributes, merged_uniforms):
        self.name = name
        self.shadertypes = list(i for i in SHADER_TYPES if i in shadertypes)
        self.attributes = attributes
        self.merged_uniforms = merged_uniforms

    def module_name(self):
        return "Program" + self.name

    def header_name(self):
        return self.module_name() + ".h"

    def source_name(self):
        return self.module_name() + ".cpp"

    def C_name(self):
        return self.module_name()

    def C_constructor(self, implem = False):
        args = ", ".join("const std::string &" + C_code_arg(i) for i in self.shadertypes)
        tmp = self.C_name() + "(" + args + ")"
        if implem:
            return self.C_name() + "::" + tmp
        return tmp

    def C_destructor(self, implem = False):
        tmp = "~" + self.C_name() + "()"
        if implem:
            return self.C_name() + "::" + tmp
        return tmp

    def C_constructor_source(self):
        emplace = "\n".join("EmplaceShader(" + STYPE_TO_GL[i] + ", " + C_code_arg(i) + ");" for i in self.shadertypes)

        attributes = "\n".join(map(C_assign_attribute, self.attributes.items()))
        uniforms   = "\n".join(map(C_assign_uniform, self.merged_uniforms.items()))

        return    self.C_constructor(True) + "\n{\n" \
                + indent( \
                      "glGenVertexArrays(1, &vao);\n" \
                    + "glGenBuffers(1, &vbo);\n\n" \
                    + emplace \
                    + "\nFinalize();\n\n" \
                    + attributes + "\n" \
                    + uniforms + "\n\n" \
                    + "glBindVertexArray(vao);\n" \
                    + "glBindBuffer(GL_ARRAY_BUFFER, vbo);\n" \
                    + C_VertexAttribPointer(self.module_name(), self.attributes) \
                    + "\n" \
                    + "\n".join("glEnableVertexAttribArray(" + name + ");" for name, _ in self.attributes.items()) \
                    + "\n\nglBindVertexArray(0);\n"
                , 1) \
                + "\n}\n"

    def C_destructor_source(self):
        return    self.C_destructor(True) + "\n{\n" \
                + indent( \
                      "glDeleteVertexArrays(1, &vao);\n" \
                    + "glDeleteBuffers(1, &vbo);" \
                , 1) \
                + "\n}\n"

    def C_members(self):
        return    "GLuint vao;\nGLuint vbo;\n\n" \
                + "\n".join(map(C_member, self.attributes.items())) \
                + "\n\n" \
                + "\n".join(map(C_member, self.merged_uniforms.items()))

    def C_setters(self):
        return "\n".join(map(lambda x: C_setter_prototype(x) + ";", self.merged_uniforms.items()))

    def C_setters_source(self):
        tmp = ""
        for name, _type in self.merged_uniforms.items():
            uniform_type = uniform_type_of(_type)
            scalar = is_scalar(_type)

            args = "(" + name + ", "
            if not scalar:
                args += "1, "
                if uniform_type[0] == "M": # Matrix
                    args += "GL_FALSE, "
            args += "_" + name
            if not scalar:
                args += ".data()"
            args += ")"

            tmp +=    C_setter_prototype((name, _type), self.C_name() + "::") \
                    + "\n{\n" \
                    + indent("glUniform" + uniform_type_of(_type) + args + ";", 1) \
                    + "\n}\n"

        return tmp

    def C_bind_source(self):
        return    C_bind_prototype(self.C_name() + "::") \
                + "\n{\n    glBindVertexArray(vao);\nglBindBuffer(GL_ARRAY_BUFFER, vbo);\n}\n"

    def C_class(self):
        v = Vertex(self.attributes)

        return    "class " + self.C_name() + " : public glutils::Program\n" \
                + "{\n    public:\n" \
                + indent(v.C_source(), 2) + "\n" \
                + indent(self.C_constructor(), 2) + ";\n" \
                + indent(self.C_destructor(), 2) + ";\n" \
                + indent(C_bind_prototype(), 2) + ";\n" \
                + indent(self.C_setters(), 2) + "\n" \
                + "    private:\n" \
                + indent(self.C_members(), 2) + "\n" \
                + "};"

    def C_header(self):
        CPP_VARIABLE = self.module_name().upper()
        return    "#ifndef " + CPP_VARIABLE + "\n" \
                + "#define " + CPP_VARIABLE + "\n\n" \
                + "#include <string>\n#include <array>\n#include <glutils.h>\n" \
                + self.C_class() + "\n" \
                + "#endif\n"

    def C_source(self):
        return    "#include <gl.h>\n#include \"" + self.header_name() + "\"\n\n" \
                + self.C_constructor_source() \
                + self.C_destructor_source() \
                + self.C_bind_source() \
                + self.C_setters_source()

class Vertex:
    def __init__(self, attributes):
        self.attributes = attributes
        
    def C_source(self):
        return    "struct Vertex\n{\n" \
                + indent("\n".join(map(C_default_attrib_member, self.attributes.items())), 1) \
                + "\n};\n"

def shader2interface(name, shader_desc, types):
    attributes_and_types = list()
    uniforms_and_types = list()

    for _type, filename in shader_desc.items():
        uniforms, attributes = extractglsl.uniforms_and_attributes_of(filename)
        if _type == STYPE_VERT:
            attributes_and_types.append(extractglsl.types_of(attributes))
        uniforms_and_types.append(extractglsl.types_of(uniforms))

    shader = Shader(name, types, extractglsl.merge(attributes_and_types), extractglsl.merge(uniforms_and_types))

    with open(shader.module_name() + ".h", "w") as f:
        f.write(shader.C_header())

    with open(shader.module_name() + ".cpp", "w") as f:
        f.write(shader.C_source())
