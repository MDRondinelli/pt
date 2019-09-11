#ifndef TRIANGLE_CL
#define TRIANGLE_CL

#include "Intersection.cl"
#include "Ray.cl"
#include "Vertex.cl"

struct Triangle {
  uint bxdf;
  uint v0;
  uint v1;
  uint v2;
};

typedef struct Triangle Triangle;

bool rayTriangle(Ray ray, float3 v0, float3 v1, float3 v2, Intersection *out) {
  float3 v1v0 = v1 - v0;
  float3 v2v0 = v2 - v0;
  float3 rov0 = ray.o - v0;
  float3 n = cross(v1v0, v2v0);
  float3 q = cross(rov0, ray.d);
  float d = 1.0f / dot(ray.d, n);
  float u = d * dot(-q, v2v0);
  float v = d * dot(q, v1v0);
  float t = d * dot(-n, rov0);

  if (u < 0.0f || u > 1.0f || v < 0.0f || u + v > 1.0f || t < ray.tmin || t > ray.tmax)
    return false;

  out->t = t;
  out->u = u;
  out->v = v;
  //out->position = ray.o + t * ray.d;
  //out->normal = normalize((1.0f - u - v) * v0.normal + u * v1.normal + v * v2.normal);

  return true;
}

float3 sampleTriangle(float3 v0, float3 v1, float3 v2, float2 random) {
  float su0 = sqrt(random.x);
  float b0 = 1.0f - su0;
  float b1 = random.x * su0;
  return b0 * v0 + b1 * v1 + (1.0f - b0 - b1) * v2;
}

#endif