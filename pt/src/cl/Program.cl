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
                 __global const uint *areaLightBuffer,
                 uint areaLightCount,
                 __global uint2 *seeds,
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

  Intersection hit;

  float3 bgColor = (float3)(0.0f);
  float3 color = (float3)(0.0f);
  float3 throughput = (float3)(1.0f);

  for (int i = 0; i < 2; ++i) {
    if (rayScene(ray, &scene, &hit)) {
      __global const Bxdf *bxdf = &bxdfBuffer[triangleBuffer[hit.triangle].bxdf];

      float3 n0 = vertexBuffer[triangleBuffer[hit.triangle].v0].normal;
      float3 n1 = vertexBuffer[triangleBuffer[hit.triangle].v1].normal;
      float3 n2 = vertexBuffer[triangleBuffer[hit.triangle].v2].normal;
      float3 position = ray.o + hit.t * ray.d;
      float3 normal = normalize((1.0f - hit.u - hit.v) * n0 + hit.u * n1 + hit.v * n2);
      
      if (areaLightCount != 0) {
        uint areaLightIndex = rand(&seeds[id]) * areaLightCount;
        uint areaLight = areaLightBuffer[areaLightIndex];

        float3 le = bxdfBuffer[triangleBuffer[areaLight].bxdf].le;
        float3 v0 = vertexBuffer[triangleBuffer[areaLight].v0].position;
        float3 v1 = vertexBuffer[triangleBuffer[areaLight].v1].position;
        float3 v2 = vertexBuffer[triangleBuffer[areaLight].v2].position;
        float lightPdf = 2.0f / length(cross(v1 - v0, v2 - v0));

        {
          float3 samplePos = sampleTriangle(v0, v1, v2, rand2(&seeds[id]));
          float3 wi = normalize(samplePos - position);
          float3 bxdfF = Bxdf_f(bxdf, normal, -ray.d, wi);
          float bxdfPdf = Bxdf_pdf(bxdf, normal, -ray.d, wi);

          float weight = lightPdf / (lightPdf * lightPdf + bxdfPdf * bxdfPdf);

          Ray shadowRay;
          shadowRay.o = position + normal * 0.0001f;
          shadowRay.d = wi;
          shadowRay.tmin = 0.0f;
          shadowRay.tmax = 1000.0f;

          Intersection shadowHit;
          float shadow = rayScene(shadowRay, &scene, &shadowHit) && shadowHit.triangle == areaLight;
          float attenuation = 1.0f / (shadowHit.t * shadowHit.t + 0.001f);
          
          color += 0.5f * areaLightCount * throughput * weight * shadow * attenuation * le * bxdfF * fabs(dot(normal, wi));
        }

        {
          BxdfSample sample = Bxdf_sample(bxdf, normal, -ray.d, rand2(&seeds[id]));
          float weight = sample.pdf / (sample.pdf * sample.pdf + lightPdf * lightPdf);

          Ray shadowRay;
          shadowRay.o = position + normal * 0.0001f;
          shadowRay.d = sample.wi;
          shadowRay.tmin = 0.0f;
          shadowRay.tmax = 1000.0f;
          
          Intersection shadowHit;
          float shadow = rayScene(shadowRay, &scene, &shadowHit) && shadowHit.triangle == areaLight;
          float attenuation = 1.0f / (shadowHit.t * shadowHit.t + 0.001f);

          color += 0.5f * areaLightCount * throughput * shadow * attenuation * le * weight * sample.f * fabs(dot(normal, sample.wi)); 
        }
      }

      BxdfSample sample = Bxdf_sample(bxdf, normal, -ray.d, rand2(&seeds[id]));
      throughput *= sample.f / sample.pdf * fabs(dot(normal, sample.wi));

      ray.o = position + normal * 0.0001f;
      ray.d = sample.wi;
    } else {
      color += throughput * bgColor;
      break;
    }
  }

  results[id] = color;
}