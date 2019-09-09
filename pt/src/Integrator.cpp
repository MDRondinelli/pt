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

Integrator::Integrator()
    : mProgram{createProgram()}, mKernel{mProgram, "Li"},
      mRng{std::random_device{}()} {}

void Integrator::Li(const Scene &scene, std::vector<Ray> &rays,
                    std::vector<glm::vec4> &results) {
  std::vector<glm::uvec2> seeds;
  seeds.reserve(rays.size());

  for (size_t i = 0; i < rays.size(); ++i)
    seeds.emplace_back(mRng(), mRng());

  cl::Buffer dSeeds{seeds.begin(), seeds.end(), false};
  cl::Buffer dRays{rays.begin(), rays.end(), true};
  cl::Buffer dResults{CL_MEM_WRITE_ONLY, sizeof(glm::vec4) * rays.size()};

  cl::NDRange global{rays.size()};
  cl::EnqueueArgs enqueueArgs{global};
  auto event = mKernel(enqueueArgs, scene.getBxdfBuffer(),
                       scene.getVertexBuffer(), scene.getTriangleBuffer(),
                       scene.getTriangleCount(), dSeeds, dRays, dResults);

  auto begin = std::chrono::high_resolution_clock::now();
  event.wait();
  auto end = std::chrono::high_resolution_clock::now();

  std::chrono::duration<float> duration = end - begin;
  std::cout << "Took " << duration.count() << "s\n";

  results.resize(rays.size());
  cl::copy(dResults, results.begin(), results.end());
}
} // namespace pt