#pragma once

#include <cstdint>
#include <glm/glm.hpp>

using FnKernel = void (*)(glm::vec4& gl_FragColor, const glm::vec2 gl_FragCoord, float time, const glm::vec2 resolution);

void RunKernel(std::int32_t width, std::int32_t height, void* p, std::int32_t stride, float time, FnKernel fnKernel);
void RunKernelParallel(std::int32_t width, std::int32_t height, void* p, std::int32_t stride, float time, FnKernel fnKernel);
