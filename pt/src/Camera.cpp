#include "Camera.h"

namespace pt {
Camera::Camera(const glm::mat4 &transform, float fov, float aspect)
    : transform{transform},
      fov{fov},
      aspect{aspect} {}

Ray Camera::sample(glm::vec2 position) const {
  glm::vec3 o{0.0f};
  o = glm::vec3{transform * glm::vec4(o, 1.0f)};

  auto tanHalfFov = glm::tan(fov * 0.5f);
  glm::vec3 d{(position.x * 2.0f - 1.0f) * tanHalfFov * aspect,
                     (position.y * 2.0f - 1.0f) * tanHalfFov, -1.0f};
  d = glm::normalize(d);
  d = glm::vec3{transform * glm::vec4(d, 0.0f)};

  return Ray{o, d, 0.001f, 1000.0f};
}
} // namespace pt