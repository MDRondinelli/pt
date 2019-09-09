#include "Film.h"

namespace pt {
Film::Film(glm::ivec2 size)
    : mSize{size}, mPixels(mSize.x * mSize.y, glm::vec4{0.0f}) {}

void Film::expose(glm::ivec2 position, glm::vec3 color) {
  mPixels[mSize.x * position.y + position.x] += glm::vec4{color, 1.0f};
}

Image Film::develop() const {
  auto tonemap = [](glm::vec3 x) {
    x *= 0.6f;
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return glm::clamp((x * (a * x + b)) / (x * (c * x + d) + e),
                      glm::vec3{0.0f}, glm::vec3{1.0f});
  };

  Image ret{mSize, true};
  for (auto y = 0; y < mSize.y; ++y) {
    for (auto x = 0; x < mSize.x; ++x) {
      glm::vec3 color = mPixels[mSize.x * y + x];
      color /= mPixels[mSize.x * y + x].a;
      color = tonemap(color);

      ret.store({x, y}, glm::vec4{color, 1.0f});
    }
  }

  return ret;
}

glm::ivec2 Film::getSize() const { return mSize; }
} // namespace pt