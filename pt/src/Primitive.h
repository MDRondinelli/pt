#pragma once

#include <memory>

#include "BxDF.h"
#include "Mesh.h"

namespace pt {
class Primitive {
  std::shared_ptr<Mesh> mMesh;
  std::shared_ptr<Bxdf> mBxdf;
  glm::mat4 mTransform;

public:
  Primitive(std::shared_ptr<Mesh> mesh, std::shared_ptr<Bxdf> bxdf,
            const glm::mat4 &transform);

  std::shared_ptr<Mesh> getMesh() const;
  std::shared_ptr<Bxdf> getBxdf() const;
  const glm::mat4 &getTransform() const;
};
} // namespace pt