import re

endline = re.compile("(\r?\n)+")
ignore = re.compile(r"^\s*#")
uniform = re.compile(r"uniform\s+([^;]+)\s*;")
attribute = re.compile(r"in\s+([^;]+)\s*;")

separator = re.compile(r"\s+")

def uniforms_and_attributes_of(filename):
    with open(filename, "r") as f:
        content = ""
        for line in f:
            if not ignore.match(line):
                content += endline.sub("", line)
        return uniform.findall(content), attribute.findall(content)

def types_of(symbols):
    ret = dict()
    for uniform in symbols:
        t = separator.split(uniform)
        name = t[-1]
        _type = t[0]
        ret[name] = _type
    return ret

def merge(symbols_and_types):
    ret = dict()
    for uat in symbols_and_types:
        for name, _type in uat.items():
            if name in ret.keys():
                if ret[name] != _type:
                    raise Exception("merge_uniforms: %s has at least two different types!" % name)
            else:
                ret[name] = _type
    return ret
