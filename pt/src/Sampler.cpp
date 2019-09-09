#include "Sampler.h"

#include <algorithm>

namespace pt {
Sampler::Sampler(size_t pixelCount, size_t xSampleCount, size_t ySampleCount)
    : mPixelCount{pixelCount}, mPixelIndex{0},
      mSampleCount{xSampleCount * ySampleCount}, mSampleIndex{0} {
  mSamples.reserve(mPixelCount * mSampleCount);

  std::uniform_real_distribution<float> d{0.0f, 1.0f};
  for (size_t i = 0; i < mPixelCount; ++i) {
    for (size_t x = 0; x < xSampleCount; ++x) {
      for (size_t y = 0; y < ySampleCount; ++y) {
        mSamples.emplace_back((x + d(mRng)) / xSampleCount,
                              (y + d(mRng)) / ySampleCount);
      }
    }

    auto first = mSamples.end() - mSampleCount;
    auto last = mSamples.end();
    std::shuffle(first, last, mRng);
  }
}

glm::vec2 Sampler::sample() {
  auto ret = mSamples[mSampleCount * mPixelIndex + mSampleIndex++];
  
  if (mSampleIndex == mSampleCount) {
    std::uniform_int_distribution<size_t> d{0, mPixelCount - 1};
    mPixelIndex = d(mRng);
    mSampleIndex = 0;
  }

  return ret;
}

size_t Sampler::getPixelCount() const { return mPixelCount; }

size_t Sampler::getSampleCount() const { return mSampleCount; }
} // namespace pt