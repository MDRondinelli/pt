#pragma once

#include <cstddef>

#include <glm/glm.hpp>

namespace pt {
struct Plane {
  alignas(16) glm::vec3 c;
  alignas(16) glm::vec3 n;
};

static_assert(sizeof(Plane) == 32);
static_assert(offsetof(Plane, c) == 0);
static_assert(offsetof(Plane, n) == 16);
} // namespace pt