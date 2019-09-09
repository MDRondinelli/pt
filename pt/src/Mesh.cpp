#include "Mesh.h"

#include <cstdlib>

#include <iostream>
#include <unordered_map>

#include <glm/gtx/hash.hpp>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace pt {
Mesh::Mesh(const std::filesystem::path &path) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn;
  std::string err;

  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
                        path.string().c_str())) {
    std::cout << warn << "\n" << err << "\n";
    std::exit(0);
  }

  std::unordered_map<glm::uvec2, uint32_t> indexMap;

  for (auto &shape : shapes) {
    for (auto &index : shape.mesh.indices) {
      glm::uvec2 objIndex{index.vertex_index, index.normal_index};
      auto it = indexMap.find(objIndex);
      if (it != indexMap.end()) {
        mIndices.emplace_back(it->second);
      } else {
        Vertex vertex;
        vertex.position = {attrib.vertices[3 * index.vertex_index + 0],
                           attrib.vertices[3 * index.vertex_index + 1],
                           attrib.vertices[3 * index.vertex_index + 2]};
        vertex.normal = {attrib.normals[3 * index.normal_index + 0],
                         attrib.normals[3 * index.normal_index + 1],
                         attrib.normals[3 * index.normal_index + 2]};
        indexMap.emplace(objIndex, mVertices.size());
        mIndices.emplace_back(mVertices.size());
        mVertices.emplace_back(vertex);
      }
    }
  }
}

const std::vector<Vertex> &Mesh::getVertices() const { return mVertices; }

const std::vector<uint32_t> &Mesh::getIndices() const { return mIndices; }
} // namespace pt