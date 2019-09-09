#pragma once

#include <random>

#include <CL/cl2.hpp>

#include "Ray.h"
#include "Scene.h"

namespace pt {
class Integrator {
  size_t mMaxItems;
  cl::Buffer mSeedBuffer;
  cl::Buffer mRayBuffer;
  cl::Buffer mResultBuffer;

  cl::Program mProgram;
  cl::KernelFunctor<cl::Buffer, cl::Buffer, cl::Buffer, cl_uint, cl::Buffer,
                    cl::Buffer, cl::Buffer>
      mKernel;

  std::mt19937 mRng;

public:
  Integrator(size_t maxItems);

  void Li(const Scene &scene, std::vector<Ray> &rays,
          std::vector<glm::vec4> &results);

  size_t getMaxItems() const;
};
} // namespace pt