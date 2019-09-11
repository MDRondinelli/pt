#pragma once

#include <vector>

#include <CL/cl2.hpp>

#include "Camera.h"
#include "Primitive.h"

namespace pt {
class Scene {
  std::vector<std::shared_ptr<Primitive>> mPrimitives;
  std::shared_ptr<Camera> mCamera;
  
  cl::Buffer mBxdfBuffer;
  cl::Buffer mVertexBuffer;
  cl::Buffer mTriangleBuffer;
  uint32_t mTriangleCount;
  cl::Buffer mAreaLightBuffer;
  uint32_t mAreaLightCount;

public:
  void add(std::shared_ptr<Primitive> primitive);
  void remove(std::shared_ptr<Primitive> primitive);

  std::shared_ptr<Camera> getCamera() const;
  void setCamera(std::shared_ptr<Camera> camera);

  void updateDeviceRepresentation();

  const cl::Buffer &getBxdfBuffer() const;
  const cl::Buffer &getVertexBuffer() const;
  const cl::Buffer &getTriangleBuffer() const;
  uint32_t getTriangleCount() const;
  const cl::Buffer &getAreaLightBuffer() const;
  uint32_t getAreaLightCount() const;
};
} // namespace pt