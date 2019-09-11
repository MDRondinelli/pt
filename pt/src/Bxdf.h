#pragma once

#include <cstddef>

#include <glm/glm.hpp>

namespace pt {
struct DiffuseBrdf {
  alignas(16) glm::vec3 color;
};

static_assert(sizeof(DiffuseBrdf) == 16);
static_assert(offsetof(DiffuseBrdf, color) == 0);

struct SpecularBrdf {
  alignas(16) glm::vec3 color;
};

static_assert(sizeof(SpecularBrdf) == 16);
static_assert(offsetof(SpecularBrdf, color) == 0);

struct SpecularBtdf {
  alignas(16) glm::vec3 color;
  alignas(16) float etaA;
  float etaB;
};

static_assert(sizeof(SpecularBtdf) == 32);
static_assert(offsetof(SpecularBtdf, color) == 0);
static_assert(offsetof(SpecularBtdf, etaA) == 16);
static_assert(offsetof(SpecularBtdf, etaB) == 20);

struct SpecularBsdf {
  alignas(16) glm::vec3 reflectance;
  alignas(16) glm::vec3 transmittance;
  alignas(16) float etaA;
  float etaB;
};

static_assert(sizeof(SpecularBsdf) == 48);
static_assert(offsetof(SpecularBsdf, reflectance) == 0);
static_assert(offsetof(SpecularBsdf, transmittance) == 16);
static_assert(offsetof(SpecularBsdf, etaA) == 32);
static_assert(offsetof(SpecularBsdf, etaB) == 36);

enum class BxdfType : uint32_t {
  DIFFUSE_BRDF,
  SPECULAR_BRDF,
  SPECULAR_BTDF,
  SPECULAR_BSDF
};

struct Bxdf {
  BxdfType type;
  alignas(16) glm::vec3 le;
  union {
    DiffuseBrdf diffuseBrdf;
    SpecularBrdf specularBrdf;
    SpecularBtdf specularBtdf;
    SpecularBsdf specularBsdf;
  };

  Bxdf(const DiffuseBrdf &bxdf, glm::vec3 le = glm::vec3{0.0f})
      : type{BxdfType::DIFFUSE_BRDF}, le{le}, diffuseBrdf{bxdf} {}

  Bxdf(const SpecularBrdf &bxdf, glm::vec3 le = glm::vec3{0.0f})
      : type{BxdfType::SPECULAR_BRDF}, le{le}, specularBrdf{bxdf} {}

  Bxdf(const SpecularBtdf &bxdf, glm::vec3 le = glm::vec3{0.0f})
      : type{BxdfType::SPECULAR_BTDF}, le{le}, specularBtdf{bxdf} {}

  Bxdf(const SpecularBsdf &bxdf, glm::vec3 le = glm::vec3{0.0f})
      : type{BxdfType::SPECULAR_BSDF}, le{le}, specularBsdf{bxdf} {}
};

static_assert(sizeof(Bxdf) == 80);
static_assert(offsetof(Bxdf, le) == 16);
static_assert(offsetof(Bxdf, diffuseBrdf) == 32);
static_assert(offsetof(Bxdf, specularBrdf) == 32);
static_assert(offsetof(Bxdf, specularBtdf) == 32);
static_assert(offsetof(Bxdf, specularBsdf) == 32);
} // namespace pt