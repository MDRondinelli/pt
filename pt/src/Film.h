#pragma once

#include "Image.h"

namespace pt {
class Film {
  glm::ivec2 mSize;
  std::vector<glm::vec4> mPixels;

public:
  Film(glm::ivec2 size);

  void expose(glm::ivec2 position, glm::vec3 color);
  Image develop() const;

  glm::ivec2 getSize() const;
};
} // namespace pt