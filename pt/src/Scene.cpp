#include "Scene.h"

#include <unordered_map>

#include "Triangle.h"

namespace pt {
void Scene::add(std::shared_ptr<Primitive> primitive) {
  mPrimitives.emplace_back(std::move(primitive));
}

void Scene::remove(std::shared_ptr<Primitive> primitive) {
  auto it = std::find(mPrimitives.begin(), mPrimitives.end(), primitive);
  if (it != mPrimitives.end())
    mPrimitives.erase(it);
}

std::shared_ptr<Camera> Scene::getCamera() const { return mCamera; }

void Scene::setCamera(std::shared_ptr<Camera> camera) {
  mCamera = std::move(camera);
}

void Scene::updateDeviceRepresentation() {
  std::vector<Bxdf> bxdfs;
  std::unordered_map<Bxdf *, uint32_t> bxdfMap;
  size_t vertexCount = 0;
  size_t indexCount = 0;

  for (auto &primitive : mPrimitives) {
    auto &bxdf = *primitive->getBxdf();

    if (bxdfMap.find(&bxdf) == bxdfMap.end()) {
      bxdfMap.emplace(&bxdf, bxdfs.size());
      bxdfs.emplace_back(bxdf);
    }

    auto &mesh = *primitive->getMesh();
    vertexCount += mesh.getVertices().size();
    indexCount += mesh.getIndices().size();
  }

  std::vector<Vertex> vertices;
  vertices.reserve(vertexCount);

  std::vector<Triangle> triangles;
  triangles.reserve(indexCount / 3);

  std::vector<uint32_t> areaLights;

  for (auto &primitive : mPrimitives) {
    auto &mesh = *primitive->getMesh();
    auto &meshVertices = mesh.getVertices();
    auto &meshIndices = mesh.getIndices();
    auto &bxdf = *primitive->getBxdf();
    auto &transform = primitive->getTransform();

    for (size_t i = 0; i < meshIndices.size(); i += 3) {
      Triangle triangle;
      triangle.bxdf = bxdfMap.at(&bxdf);
      triangle.v0 = vertices.size() + meshIndices[i + 0];
      triangle.v1 = vertices.size() + meshIndices[i + 1];
      triangle.v2 = vertices.size() + meshIndices[i + 2];
      triangles.emplace_back(triangle);

      if (bxdf.le != glm::vec3{0.0f})
        areaLights.emplace_back(i / 3);
    }

    for (auto vertex : meshVertices) {
      vertex.position = transform * glm::vec4{vertex.position, 1.0f};
      vertex.normal = transform * glm::vec4{vertex.normal, 0.0f};
      vertices.emplace_back(vertex);
    }
  }

  mBxdfBuffer = cl::Buffer{bxdfs.begin(), bxdfs.end(), true};
  mVertexBuffer = cl::Buffer{vertices.begin(), vertices.end(), true};
  mTriangleBuffer = cl::Buffer{triangles.begin(), triangles.end(), true};
  mTriangleCount = triangles.size();
  mAreaLightBuffer = cl::Buffer{areaLights.begin(), areaLights.end(), true};
  mAreaLightCount = areaLights.size();
}

const cl::Buffer &Scene::getBxdfBuffer() const { return mBxdfBuffer; }

const cl::Buffer &Scene::getVertexBuffer() const { return mVertexBuffer; }

const cl::Buffer &Scene::getTriangleBuffer() const { return mTriangleBuffer; }

uint32_t Scene::getTriangleCount() const { return mTriangleCount; }

const cl::Buffer &Scene::getAreaLightBuffer() const { return mAreaLightBuffer; }

uint32_t Scene::getAreaLightCount() const { return mAreaLightCount; }
} // namespace pt