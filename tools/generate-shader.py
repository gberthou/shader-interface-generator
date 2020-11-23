import sys
import os
import json
from shader2interface import *

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

with open(sys.argv[1], "r") as f:
    os.chdir("shaders")
    data = json.load(f)
    types = list({STYPE_VERT, STYPE_FRAG, STYPE_GEO, STYPE_TCS, STYPE_TES} & data["shaders"].keys())
    shader2interface(data["name"], data["shaders"], types)
