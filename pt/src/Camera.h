#pragma once

#include <glm/ext.hpp>

#include "Ray.h"

namespace pt {
class Camera {
  glm::mat4 mTransform;
  float mFov;
  float mAspect;
  float mAperture;
  float mFocusDistance;

public:
  Camera(const glm::mat4 &transform, float fov, float aspect, float aperture,
         float focusDistance);
  Camera(const glm::mat4 &transform, float fov, float aspect);

  Ray sample(glm::vec2 position, glm::vec2 random) const;
};
} // namespace pt