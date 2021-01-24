def indent(s, n):
    lines = s.split("\n")
    return "\n".join(("    " * n) + line for line in lines)

def C_program_getter(name, index, prefix = ""):
    return "const " + name + " &" + prefix + "GetProgram" + str(index) + "() const"

def C_fb_getter(index, prefix = ""):
    return "const Framebuffer &" + prefix + "GetFramebuffer" + str(index) + "() const"

def C_draw_function(index, prefix = ""):
    return "void " + prefix + "Draw" + str(index) + "(const std::function<void()> &draw) const"

class Renderer:
    def __init__(self, name, shaderlist, render_order):
        self.name = name
        self.shaderlist = shaderlist
        self.render_order = render_order

    def module_name(self):
        return "Program" + self.name

    def header_name(self):
        return self.module_name() + ".h"

    def source_name(self):
        return self.module_name() + ".cpp"

    def C_name(self):
        return self.module_name()

    def C_constructor(self, implem = False):
        if implem:
            prefix = self.C_name() + "::"
        else:
            prefix = ""

        args = ", ".join("GLsizei width" + str(i) + ", GLsizei height" + str(i) for i in range(len(self.render_order)-1))
        args += ",\n" + ",\n".join(s.C_constructor_args(str(i)) for i, s in enumerate(self.shaderlist))
        
        return prefix + self.C_name() + "(" + args + ")"

    def C_program_getters(self):
        return "\n".join(C_program_getter(s.C_name(), i) + ";" for i, s in enumerate(self.shaderlist))

    def C_fb_getters(self):
        return "\n".join(C_fb_getter(i) + ";" for i in range(len(self.render_order)-1))

    def C_draw_functions(self):
        return "\n".join(C_draw_function(i) + ";" for i in range(len(self.render_order)))

    def C_class(self):
        program_members = "\n".join(s.C_name() + " m" + s.C_name() + ";" for s in self.shaderlist)
        framebuffer_members = "\n".join("Framebuffer fb" + str(i) + ";" for i in range(len(self.render_order) - 1))

        return    "class " + self.C_name() \
                + "\n{\n    public:\n" \
                + indent(self.C_constructor(), 2) + ";\n" \
                + indent(self.C_program_getters(), 2) + "\n" \
                + indent(self.C_fb_getters(), 2) + "\n" \
                + indent(self.C_draw_functions(), 2) + "\n" \
                + "    private:\n" \
                + indent(program_members, 2) + "\n" \
                + indent(framebuffer_members, 2) + "\n" \
                + "};"

    def C_constructor_source(self):
        ret = self.C_constructor(True) + ":\n"
        ret += indent(",\n".join("m" + s.C_name() + "(" + s.C_constructor_args(str(i), True) + ")" for i, s in enumerate(self.shaderlist)), 1)
        ret += ",\n"
        ret += indent(",\n".join("fb" + str(i) + "(width" + str(i) + ", height" + str(i) + ", 1)" for i in range(len(self.render_order)-1)), 1)
        ret += "\n{\n}\n"
        return ret

    def C_program_getters_source(self):
        return "\n".join(C_program_getter(s.C_name(), i, self.C_name() + "::") + "\n{\n    return m" + s.C_name() + ";\n}\n" for i, s in enumerate(self.shaderlist))

    def C_fb_getters_source(self):
        return "\n".join(C_fb_getter(i, self.C_name() + "::") + "\n{\n    return fb" + str(i) + ";\n}\n" for i in range(len(self.render_order)-1))

    def C_draw_functions_source(self):
        ret = ""
        for i, pname in enumerate(self.render_order):
            shader = next(s for s in self.shaderlist if s.name == pname)

            programname = "m" + shader.C_name()
            last = (i == len(self.render_order) - 1)

            ret +=    C_draw_function(i, self.C_name() + "::") \
                    + "\n{\n" \
                    + indent(programname + ".Apply();\n" \
                           + programname + ".Bind();", 1)
            if not last:
                ret += "\n    fb" + str(i) + ".Bind();"
            ret += "\n    draw();\n"
            if not last:
                ret += "    fb" + str(i) + ".Unbind();\n"
            ret += "}\n"
        return ret

    def C_header_body(self):
        return self.C_class()

    def C_source_body(self):
        return    self.C_constructor_source() \
                + self.C_program_getters_source() \
                + self.C_fb_getters_source() \
                + self.C_draw_functions_source()

def to_renderer(name, shaderlist, render_order):
    return Renderer(name, shaderlist, render_order)
