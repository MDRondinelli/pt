#ifndef SCENE_CL
#define SCENE_CL

#include "Bxdf.cl"
#include "Triangle.cl"

struct Scene {
  __global const Bxdf *bxdfBuffer;
  __global const Vertex *vertexBuffer;
  __global const Triangle *triangleBuffer;
  uint triangleCount;
  __global const uint *areaLightBuffer;
  uint areaLightCount;
};

typedef struct Scene Scene;

bool rayScene(Ray ray, const Scene *scene, Intersection *out) {
  bool ret = false;
  Intersection hit;
  
  for (uint i = 0; i < scene->triangleCount; ++i) {
    if (rayTriangle(ray,
                    scene->vertexBuffer[scene->triangleBuffer[i].v0].position,
                    scene->vertexBuffer[scene->triangleBuffer[i].v1].position,
                    scene->vertexBuffer[scene->triangleBuffer[i].v2].position,
                    &hit) && (!ret || hit.t < out->t)) {
      ret = true;
      *out = hit;
      out->triangle = i;
    }
  }
  
  return ret;
}

#endif