#pragma once

#include "Ray.h"

namespace pt {
struct Camera {
  alignas(16) glm::mat4 transform;
  float fov;
  float aspect;

  Camera(const glm::mat4 &transform, float fov, float aspect);
  Ray sample(glm::vec2 position) const;
};

static_assert(sizeof(Camera) == 80);
static_assert(offsetof(Camera, transform) == 0);
static_assert(offsetof(Camera, fov) == 64);
static_assert(offsetof(Camera, aspect) == 68);
} // namespace pt