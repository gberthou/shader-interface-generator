#include <string>
#include <fstream>

#include "utils.h"

std::string FileContents(const std::string &filename)
{
    std::ifstream file(filename.c_str());
    return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}

