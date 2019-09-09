#pragma once

#include <filesystem>
#include <vector>

#include "Vertex.h"

namespace pt {
class Mesh {
  std::vector<Vertex> mVertices;
  std::vector<uint32_t> mIndices;

public:
  Mesh(const std::filesystem::path &path);

  const std::vector<Vertex> &getVertices() const;
  const std::vector<uint32_t> &getIndices() const;
};
} // namespace pt