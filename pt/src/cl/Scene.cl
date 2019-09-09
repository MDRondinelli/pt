#ifndef SCENE_CL
#define SCENE_CL

#include "Bxdf.cl"
#include "Triangle.cl"

struct Scene {
  __global const Bxdf *bxdfBuffer;
  __global const Vertex *vertexBuffer;
  __global const Triangle *triangleBuffer;
  uint triangleCount;
};

typedef struct Scene Scene;

bool rayScene(Ray ray, const Scene *scene, Intersection *out) {
  bool ret = false;
  Intersection hit;
  
  for (uint i = 0; i < scene->triangleCount; ++i) {
    if (rayTriangle(ray,
                    scene->vertexBuffer[scene->triangleBuffer[i].v0],
                    scene->vertexBuffer[scene->triangleBuffer[i].v1],
                    scene->vertexBuffer[scene->triangleBuffer[i].v2],
                    &hit) && (!ret || hit.t < out->t)) {
      ret = true;
      *out = hit;
      out->bxdf = scene->triangleBuffer[i].bxdf;
    }
  }
  
  return ret;
}

#endif