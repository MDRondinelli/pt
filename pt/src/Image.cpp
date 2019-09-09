#include "Image.h"

namespace pt {
Image::Image(glm::ivec2 size, bool srgb)
    : mSize{size}, mSrgb{srgb}, mPixels{mSize.x * mSize.y} {}

glm::vec4 Image::sample(glm::vec2 position) const {
  position *= mSize;
  position -= 0.5f;

  auto min = glm::ivec2{glm::floor(position)};
  auto max = glm::ivec2{glm::ceil(position)};
  auto fraction = position - glm::vec2{min};
  float w0 = (1.0f - fraction.x) * (1.0f - fraction.y);
  float w1 = fraction.x * (1.0f - fraction.y);
  float w2 = (1.0f - fraction.x) * fraction.y;
  float w3 = fraction.x * fraction.y;

  min.x = (unsigned)min.x % mSize.x;
  min.y = (unsigned)min.y % mSize.y;
  max.x = (unsigned)max.x % mSize.x;
  max.y = (unsigned)max.y % mSize.y;

  glm::vec4 ret{0.0f};
  ret += glm::vec4{mPixels[mSize.x * min.y + min.x]} * w0;
  ret += glm::vec4{mPixels[mSize.x * min.y + max.x]} * w1;
  ret += glm::vec4{mPixels[mSize.x * max.y + min.x]} * w2;
  ret += glm::vec4{mPixels[mSize.x * max.y + max.x]} * w3;
  ret /= 256.0f;
  if (mSrgb) {
    ret.x = glm::pow(ret.x, 2.2f);
    ret.y = glm::pow(ret.y, 2.2f);
    ret.z = glm::pow(ret.z, 2.2f);
  }
  return ret;
}

glm::vec4 Image::load(glm::ivec2 position) const {
  glm::vec4 ret{mPixels[mSize.x * position.y + position.x]};
  ret /= 256.0f;
  if (mSrgb) {
    ret.r = glm::pow(ret.r, 2.2f);
    ret.g = glm::pow(ret.g, 2.2f);
    ret.b = glm::pow(ret.b, 2.2f);
  }
  return ret;
}

void Image::store(glm::ivec2 position, glm::vec4 color) {
  if (mSrgb) {
    color.r = glm::pow(color.r, 1.0f / 2.2f);
    color.g = glm::pow(color.g, 1.0f / 2.2f);
    color.b = glm::pow(color.b, 1.0f / 2.2f);
  }

  color *= 256.0f;
  color = glm::clamp(color, glm::vec4{0.0f}, glm::vec4{255.0f});
  mPixels[mSize.x * position.y + position.x] = color;
}

glm::ivec2 Image::getSize() const { return mSize; }

bool Image::isSrgb() const { return mSrgb; }
} // namespace pt