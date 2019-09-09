#include "Camera.h"

namespace pt {
Camera::Camera(const glm::mat4 &transform, float fov, float aspect,
               float aperture, float focusDistance)
    : mTransform{transform}, mFov{fov}, mAspect{aspect}, mAperture{aperture},
      mFocusDistance{focusDistance} {}

Camera::Camera(const glm::mat4 &transform, float fov, float aspect)
    : Camera(transform, fov, aspect, 0.0f, 1.0f) {}

Ray Camera::sample(glm::vec2 position, glm::vec2 random) const {
  auto tanHalfFov = std::tan(mFov * 0.5f);
  glm::vec3 endpoint{(position.x * 2.0f - 1.0f) * tanHalfFov * mAspect,
                     (position.y * 2.0f - 1.0f) * tanHalfFov, -1.0f};
  endpoint *= mFocusDistance;
  glm::vec3 o{0.0f};
  glm::vec3 d{glm::normalize(endpoint - o)};
  o = glm::vec3{mTransform * glm::vec4(o, 1.0f)};
  d = glm::vec3{mTransform * glm::vec4(d, 0.0f)};
  return Ray{o, d, 0.001f, 1000.0f};
}
} // namespace pt