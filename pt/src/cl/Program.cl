#include "Random.cl"
#include "Scene.cl"

__kernel void Li(__global const Bxdf *bxdfBuffer,
                 __global const Vertex *vertexBuffer,
                 __global const Triangle *triangleBuffer,
                 uint triangleCount,
                 __global uint2 *seeds,
                 __global const Ray *rays,
                 __global float3 *results) {
  int id = get_global_id(0);

  Scene scene;
  scene.bxdfBuffer = bxdfBuffer;
  scene.vertexBuffer = vertexBuffer;
  scene.triangleBuffer = triangleBuffer;
  scene.triangleCount = triangleCount;

  Ray ray = rays[id];
  Intersection hit;

  float3 bgColor = (float3)(1.0f);
  float3 color = (float3)(0.0f);
  float3 throughput = (float3)(1.0f);

  for (int i = 0; i < 4; ++i) {
    if (rayScene(ray, &scene, &hit)) {
      __global const Bxdf *bxdf = &bxdfBuffer[hit.bxdf];

      BxdfSample sample = Bxdf_sample(bxdf, hit.normal, -ray.d, rand2(&seeds[id]));
      throughput *= sample.f / sample.pdf * fabs(dot(hit.normal, sample.wi));

      ray.o = hit.position;
      ray.d = sample.wi;
    } else {
      color += throughput * bgColor;
      break;
    }
  }

  results[id] = color;
}