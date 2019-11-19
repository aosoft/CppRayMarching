#pragma once

#include "kernel.h"

void Sphere(glm::vec4& gl_FragColor, const glm::vec4& gl_FragCoord, float time, const glm::vec2 resolution);
void KaleidoscopeTunnel(glm::vec4& fragColor, const glm::vec4& fragCoord, float iTime, const glm::vec2 iResolution);
void NeonRoad(glm::vec4& fragColor, const glm::vec4& fragCoord, float iTime, const glm::vec2 iResolution);
void HologramBoxes(glm::vec4& fragColor, const glm::vec4& fragCoord, float iTime, const glm::vec2 iResolution);
