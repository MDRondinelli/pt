#pragma once

#include <cstddef>

#include <glm/glm.hpp>

namespace pt {
struct Vertex {
  alignas(16) glm::vec3 position;
  alignas(16) glm::vec3 normal;
};

static_assert(sizeof(Vertex) == 32);
static_assert(offsetof(Vertex, position) == 0);
static_assert(offsetof(Vertex, normal) == 16);
} // namespace pt