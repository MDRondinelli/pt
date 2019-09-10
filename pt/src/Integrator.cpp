#include "Integrator.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
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
      mKernel{mProgram, "Li"} {
  std::mt19937 rng{std::random_device{}()};
  std::vector<glm::uvec2> seeds;
  seeds.reserve(mMaxItems);
  for (size_t i = 0; i < mMaxItems; ++i)
    seeds.emplace_back(rng(), rng());
  cl::copy(seeds.begin(), seeds.end(), mSeedBuffer);

  mResults.reserve(mMaxItems);
}

void Integrator::Li(const Scene &scene, Film &film, glm::ivec2 tileOffs,
                    glm::ivec2 tileSize, glm::ivec2 sampleCount) {
  auto rayCount = tileSize.x * tileSize.y * sampleCount.x * sampleCount.y;

  cl_int2 clImageSize{film.getSize().x, film.getSize().y};
  cl_int2 clTileOffs{tileOffs.x, tileOffs.y};
  cl_int2 clTileSize{tileSize.x, tileSize.y};
  cl_int2 clSampleCount{sampleCount.x, sampleCount.y};

  cl::NDRange global{(cl::size_type)rayCount};
  cl::EnqueueArgs enqueueArgs{global};
  auto event =
      mKernel(enqueueArgs, clImageSize, clTileOffs, clTileSize, clSampleCount,
              *scene.getCamera(), scene.getBxdfBuffer(),
              scene.getVertexBuffer(), scene.getTriangleBuffer(),
              scene.getTriangleCount(), mSeedBuffer, mRayBuffer, mResultBuffer);

  {
    auto begin = std::chrono::high_resolution_clock::now();
    event.wait();
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<float> duration = end - begin;
    //std::cout << "Kernel took " << duration.count() << "s\n";
  }

  {
    auto begin = std::chrono::high_resolution_clock::now();
    mResults.resize(rayCount);
    cl::copy(mResultBuffer, mResults.begin(), mResults.end());
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<float> duration = end - begin;
    //std::cout << "Result copy took " << duration.count() << "s\n";
  }

  {
    auto begin = std::chrono::high_resolution_clock::now();
    for (auto i = 0; i < rayCount; ++i) {
      auto pixelIndex = i / (sampleCount.x * sampleCount.y);
      auto pixelX = tileOffs.x + pixelIndex % tileSize.x;
      auto pixelY = tileOffs.y + pixelIndex / tileSize.x;
      film.expose({pixelX, pixelY}, mResults[i]);
    }
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<float> duration = end - begin;
    //std::cout << "Exposure took " << duration.count() << "s\n";
  }
}

size_t Integrator::getMaxItems() const { return mMaxItems; }
} // namespace pt