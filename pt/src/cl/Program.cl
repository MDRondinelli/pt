#include "Camera.cl"
#include "Random.cl"
#include "Scene.cl"

Ray generateRay(const Camera *camera, float2 position) {
  Ray ray;
  ray.o = (float3)(camera->transform[3].x, camera->transform[3].y, camera->transform[3].z);
  
  float tanHalfFov = tan(camera->fov * 0.5f);
  ray.d = (float3)((position.x * 2.0f - 1.0f) * tanHalfFov * camera->aspect,
                  (position.y * 2.0f - 1.0f) * tanHalfFov,
                  -1.0f);
  ray.d = normalize(ray.d);
  float4 homo = (float4)(ray.d.x, ray.d.y, ray.d.z, 0.0f);
  homo = matrixMul4(camera->transform, homo);
  ray.d = (float3)(homo.x, homo.y, homo.z);

  ray.tmin = 0.0f;
  ray.tmax = 1000.0f;

  return ray;
}

__kernel void Li(int2 imageSize,
                 int2 tileOffs,
                 int2 tileSize,
                 int2 sampleCount,
                 Camera camera,
                 __global const Bxdf *bxdfBuffer,
                 __global const Vertex *vertexBuffer,
                 __global const Triangle *triangleBuffer,
                 uint triangleCount,
                 __global uint2 *seeds,
                 __global const Ray *rays,
                 __global float3 *results) {
  int id = get_global_id(0);
  int sampleIndex = id % (sampleCount.x * sampleCount.y);
  int pixelIndex = id / (sampleCount.x * sampleCount.y);
  float sampleX = sampleIndex % sampleCount.x;
  float sampleY = sampleIndex / sampleCount.x;
  float pixelX = tileOffs.x + pixelIndex % tileSize.x;
  float pixelY = tileOffs.y + pixelIndex / tileSize.x;

  float2 samplePosition = (float2)((pixelX + (sampleX + rand(&seeds[id])) / sampleCount.x) / imageSize.x,
                                   (pixelY + (sampleY + rand(&seeds[id])) / sampleCount.y) / imageSize.y);

  Ray ray = generateRay(&camera, samplePosition);

  Scene scene;
  scene.bxdfBuffer = bxdfBuffer;
  scene.vertexBuffer = vertexBuffer;
  scene.triangleBuffer = triangleBuffer;
  scene.triangleCount = triangleCount;

  //Ray ray = rays[id];
  Intersection hit;

  float3 bgColor = (float3)(1.0f);
  float3 color = (float3)(0.0f);
  float3 throughput = (float3)(1.0f);

  for (int i = 0; i < 4; ++i) {
    if (rayScene(ray, &scene, &hit)) {
      __global const Bxdf *bxdf = &bxdfBuffer[hit.bxdf];

      BxdfSample sample = Bxdf_sample(bxdf, hit.normal, -ray.d, rand2(&seeds[id]));
      throughput *= sample.f / sample.pdf * fabs(dot(hit.normal, sample.wi));

      ray.o = hit.position + hit.normal * 0.0001f;
      ray.d = sample.wi;
    } else {
      color += throughput * bgColor;
      break;
    }
  }

  results[id] = color;
}