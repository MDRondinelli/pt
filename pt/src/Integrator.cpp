#include "Integrator.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <string>

namespace pt {
namespace {
cl::Program createProgram() {
  std::string src;

  std::ifstream in{"src/cl/Program.cl"};
  in.seekg(0, std::ios::end);
  src.resize(in.tellg());
  in.seekg(0, std::ios::beg);

  in.read(src.data(), src.size());

  cl::Program program{src};

  try {
    program.build("-I src/cl/");
  } catch (...) {
    std::cout << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(
        cl::Device::getDefault());
    std::exit(0);
  }

  return program;
}
} // namespace

Integrator::Integrator(size_t maxItems)
    : mMaxItems{maxItems},
      mSeedBuffer{CL_MEM_READ_WRITE, mMaxItems * sizeof(glm::uvec2)},
      mRayBuffer{CL_MEM_READ_ONLY, mMaxItems * sizeof(pt::Ray)},
      mResultBuffer{CL_MEM_WRITE_ONLY, mMaxItems * sizeof(glm::vec4)},
      mProgram{createProgram()},
      mKernel{mProgram, "Li"},
      mRng{std::random_device{}()} {
  std::vector<glm::uvec2> seeds;
  seeds.reserve(mMaxItems);
  for (size_t i = 0; i < mMaxItems; ++i)
    seeds.emplace_back(mRng(), mRng());
  cl::copy(seeds.begin(), seeds.end(), mSeedBuffer);
}

void Integrator::Li(const Scene &scene, std::vector<Ray> &rays,
                    std::vector<glm::vec4> &results) {
  {
    auto begin = std::chrono::high_resolution_clock::now();
    cl::copy(rays.begin(), rays.end(), mRayBuffer);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> duration = end - begin;
    std::cout << "Ray copy took " << duration.count() << "s\n";
  }

  cl::NDRange global{rays.size()};
  cl::EnqueueArgs enqueueArgs{global};
  auto event =
      mKernel(enqueueArgs, scene.getBxdfBuffer(), scene.getVertexBuffer(),
              scene.getTriangleBuffer(), scene.getTriangleCount(), mSeedBuffer,
              mRayBuffer, mResultBuffer);

  {
    auto begin = std::chrono::high_resolution_clock::now();
    event.wait();
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<float> duration = end - begin;
    std::cout << "Kernel took " << duration.count() << "s\n";
  }

  {
    auto begin = std::chrono::high_resolution_clock::now();
    results.resize(rays.size());
    cl::copy(mResultBuffer, results.begin(), results.end());
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<float> duration = end - begin;
    std::cout << "Result copy took " << duration.count() << "s\n";
  }
}

size_t Integrator::getMaxItems() const {
  return mMaxItems;
}
} // namespace pt