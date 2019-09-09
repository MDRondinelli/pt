#ifndef BXDF_CL
#define BXDF_CL

#include "Math.cl"

struct BxdfSample {
  float3 wi;
  float3 f;
  float pdf;
};

typedef struct BxdfSample BxdfSample;

struct DiffuseBrdf {
  float3 color;
};

typedef struct DiffuseBrdf DiffuseBrdf;

float3 DiffuseBrdf_f(__global const DiffuseBrdf *ptr) {
  return ptr->color * (float3)(M_1_PI);
}

float DiffuseBrdf_pdf(float3 n, float3 wi) {
  return fabs(dot(n, wi)) * M_1_PI;
}

BxdfSample DiffuseBrdf_sample(__global const DiffuseBrdf *ptr, float3 n, float2 random) {
  float r = sqrt(random.x);
  float theta = 2.0f * M_PI * random.y;
  
  float x = r * cos(theta);
  float y = r * sin(theta);
  float z = sqrt(fmax(0.0f, 1.0f - x * x - y * y));

  float3 t;
  float3 b;
  orthonormalBasis(n, &t, &b);
  
  BxdfSample ret;
  ret.wi = x * t + y * b + z * n;
  ret.f = ptr->color * (float3)(M_1_PI);
  ret.pdf = z * M_1_PI;
  return ret;
}

struct SpecularBrdf {
  float3 color;
};

typedef struct SpecularBrdf SpecularBrdf;

struct SpecularBtdf {
  float3 color;
  float etaA;
  float etaB;
};

typedef struct SpecularBtdf SpecularBtdf;

struct SpecularBsdf {
  float3 reflectance;
  float3 transmittance;
  float etaA;
  float etaB;
};

typedef struct SpecularBsdf SpecularBsdf;

enum BxdfType {
  DIFFUSE_BRDF,
  SPECULAR_BRDF,
  SPECULAR_BTDF,
  SPECULAR_BSDF
};

struct Bxdf {
  uint type;
  union {
    DiffuseBrdf diffuseBrdf;
    SpecularBrdf specularBrdf;
    SpecularBtdf specularBtdf;
    SpecularBsdf specularBsdf;
  };
};

typedef struct Bxdf Bxdf;

float3 Bxdf_f(__global const Bxdf *bxdf, float3 n, float3 wo, float3 wi) {
  switch (bxdf->type) {
  case DIFFUSE_BRDF:
    return DiffuseBrdf_f(&bxdf->diffuseBrdf);
  default:
    return (float3)(0.0f);
  }
}

float Bxdf_pdf(__global const Bxdf *bxdf, float3 n, float3 wo, float3 wi) {
  switch (bxdf->type) {
  case DIFFUSE_BRDF:
    return DiffuseBrdf_pdf(n, wi);
  default:
    return 0.0f;
  }
}

BxdfSample Bxdf_sample(__global const Bxdf *bxdf, float3 n, float3 wo, float2 random) {
  switch (bxdf->type) {
  case DIFFUSE_BRDF:
    return DiffuseBrdf_sample(&bxdf->diffuseBrdf, n, random);
  default: {
    BxdfSample ret;
    ret.wi = (float3)(0.0f);
    ret.f = (float3)(0.0f);
    ret.wi = 0.0f;
    return ret;
  }
  }
}

#endif