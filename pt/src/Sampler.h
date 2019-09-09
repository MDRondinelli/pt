#pragma once

#include <random>
#include <vector>

#include <glm/glm.hpp>

namespace pt {
class Sampler {
  size_t mPixelCount;
  size_t mPixelIndex;
  size_t mSampleCount;
  size_t mSampleIndex;
  std::vector<glm::vec2> mSamples;

  std::mt19937_64 mRng;

public:
  Sampler(size_t pixelCount, size_t xSampleCount, size_t ySampleCount);

  glm::vec2 sample();

  size_t getPixelCount() const;
  size_t getSampleCount() const;
};
} // namespace pt