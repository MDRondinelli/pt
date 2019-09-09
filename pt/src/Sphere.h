#pragma once

#include <cstddef>

#include <glm/glm.hpp>

namespace pt {
struct Sphere {
  alignas(16) glm::vec3 c;
  alignas(16) float r;
};

static_assert(sizeof(Sphere) == 32);
static_assert(offsetof(Sphere, c) == 0);
static_assert(offsetof(Sphere, r) == 16);
} // namespace pt