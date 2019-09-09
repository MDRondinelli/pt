#pragma once

#include <vector>

#include <glm/glm.hpp>

namespace pt {
class Image {
  glm::ivec2 mSize;
  bool mSrgb;
  std::vector<glm::u8vec4> mPixels;

public:
  Image(glm::ivec2 size, bool srgb);

  glm::vec4 sample(glm::vec2 position) const;
  glm::vec4 load(glm::ivec2 position) const;
  void store(glm::ivec2 position, glm::vec4 color);

  glm::ivec2 getSize() const;

  bool isSrgb() const;
};
} // namespace pt