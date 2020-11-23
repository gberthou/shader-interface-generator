#ifndef GLMUTILS_H
#define GLMUTILS_H

#include <array>
#include <glm/gtc/type_ptr.hpp>

template<int N, int M, glm::qualifier Q>
static std::array<float, N*M> mat2array(const glm::mat<N, M, float, Q> &mat)
{
    std::array<float, N*M> ret;
    const auto *data = glm::value_ptr(mat);
    std::copy(data, data + (N*M), ret.begin());
    return ret;
}

#endif

