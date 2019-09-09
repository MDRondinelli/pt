#pragma once

#include <cstddef>
#include <cstdint>

namespace pt {
struct Triangle {
  uint32_t bxdf;
  uint32_t v0;
  uint32_t v1;
  uint32_t v2;
};

static_assert(sizeof(Triangle) == 16);
static_assert(offsetof(Triangle, bxdf) == 0);
static_assert(offsetof(Triangle, v0) == 4);
static_assert(offsetof(Triangle, v1) == 8);
static_assert(offsetof(Triangle, v2) == 12);
} // namespace pt