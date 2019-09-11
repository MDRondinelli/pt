#pragma once

#include <CL/cl2.hpp>

#include "Film.h"
#include "Ray.h"
#include "Scene.h"

namespace pt {
class Integrator {
  size_t mMaxItems;
  cl::Buffer mSeedBuffer;
  cl::Buffer mResultBuffer;

  cl::Program mProgram;
  cl::KernelFunctor<cl_int2, cl_int2, cl_int2, cl_int2, Camera, cl::Buffer,
                    cl::Buffer, cl::Buffer, cl_uint, cl::Buffer, cl_uint,
                    cl::Buffer, cl::Buffer>
      mKernel;

  std::vector<glm::vec4> mResults;

public:
  Integrator(size_t maxItems);

  void Li(const Scene &scene, Film &film, glm::ivec2 tileOffs,
          glm::ivec2 tileSize, glm::ivec2 sampleCount);

  size_t getMaxItems() const;
};
} // namespace pt