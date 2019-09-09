#include "Primitive.h"

namespace pt {
Primitive::Primitive(std::shared_ptr<Mesh> mesh, std::shared_ptr<Bxdf> bxdf,
                     const glm::mat4 &transform)
    : mMesh{std::move(mesh)}, mBxdf{std::move(bxdf)}, mTransform{transform} {}

std::shared_ptr<Mesh> Primitive::getMesh() const { return mMesh; }

std::shared_ptr<Bxdf> Primitive::getBxdf() const { return mBxdf; }

const glm::mat4 &Primitive::getTransform() const { return mTransform; }
} // namespace pt