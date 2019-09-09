#pragma once

#include <glm/glm.hpp>

namespace pt {
struct Ray {
  alignas(16) glm::vec3 o;
  alignas(16) glm::vec3 d;
  alignas(16) float tmin;
  float tmax;
};

static_assert(sizeof(Ray) == 48);
static_assert(offsetof(Ray, o) == 0);
static_assert(offsetof(Ray, d) == 16);
static_assert(offsetof(Ray, tmin) == 32);
static_assert(offsetof(Ray, tmax) == 36);
} // namespace pt