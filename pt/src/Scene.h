#pragma once

#include <vector>

#include <CL/cl2.hpp>

#include "Primitive.h"

namespace pt {
class Scene {
  std::vector<std::shared_ptr<Primitive>> mPrimitives;
  cl::Buffer mBxdfBuffer;
  cl::Buffer mVertexBuffer;
  cl::Buffer mTriangleBuffer;
  uint32_t mTriangleCount;

public:
  void add(std::shared_ptr<Primitive> primitive);
  void remove(std::shared_ptr<Primitive> primitive);

  void updateDeviceRepresentation();

  const cl::Buffer &getBxdfBuffer() const;
  const cl::Buffer &getVertexBuffer() const;
  const cl::Buffer &getTriangleBuffer() const;
  uint32_t getTriangleCount() const;
};
} // namespace pt