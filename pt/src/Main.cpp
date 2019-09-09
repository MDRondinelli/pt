#include <chrono>
#include <fstream>
#include <iostream>

#include "Camera.h"
#include "Film.h"
#include "Integrator.h"
#include "Sampler.h"

int main() {
  pt::Scene scene;

  auto monkeyMesh = std::make_shared<pt::Mesh>("res/cornell.obj");
  auto monkeyBxdf =
      std::make_shared<pt::Bxdf>(pt::DiffuseBrdf{glm::vec3{0.9f}});
  auto monkeyTransform = glm::identity<glm::mat4>();
  auto monkeyPrimitive =
      std::make_shared<pt::Primitive>(monkeyMesh, monkeyBxdf, monkeyTransform);

  scene.add(monkeyPrimitive);
  scene.updateDeviceRepresentation();

  glm::vec3 eye{0.0f, 0.0f, 4.0f};
  glm::vec3 center{0.0f, 0.0f, 0.0f};
  glm::vec3 up{0.0f, 1.0f, 0.0f};

  auto localZ = glm::normalize(eye - center);
  auto localX = glm::normalize(glm::cross(up, localZ));
  auto localY = glm::cross(localZ, localX);
  glm::mat4 cameraTransform;
  cameraTransform[0] = glm::vec4{localX, 0.0f};
  cameraTransform[1] = glm::vec4{localY, 0.0f};
  cameraTransform[2] = glm::vec4{localZ, 0.0f};
  cameraTransform[3] = glm::vec4{eye, 1.0f};

  pt::Film film{{512, 512}};
  pt::Camera camera{cameraTransform, glm::radians(45.0f),
                    (float)film.getSize().x / (float)film.getSize().y};

  pt::Sampler sampler{1024, 32, 32};
  pt::Integrator integrator{64 * 64 * sampler.getSampleCount()};

  std::vector<pt::Ray> rays;
  rays.reserve(integrator.getMaxItems());

  std::vector<glm::vec4> results;
  results.reserve(integrator.getMaxItems());

  for (auto tileY = 0; tileY < film.getSize().y; tileY += 64) {
    auto minY = tileY;
    auto maxY = glm::min(tileY + 64, film.getSize().y);

    for (auto tileX = 0; tileX < film.getSize().x; tileX += 64) {
      auto minX = tileX;
      auto maxX = glm::min(tileX + 64, film.getSize().x);

      rays.clear();
      results.clear();

      for (auto y = minY; y < maxY; ++y) {
        for (auto x = minX; x < maxX; ++x) {
          for (auto i = 0; i < sampler.getSampleCount(); ++i) {
            auto random = sampler.sample();
            glm::vec2 position;
            position.x = (x + random.x) / film.getSize().x;
            position.y = (y + random.y) / film.getSize().y;

            rays.emplace_back(camera.sample(position, glm::vec2{0.0f}));
          }
        }
      }

      integrator.Li(scene, rays, results);

      for (auto y = minY; y < maxY; ++y)
        for (auto x = minX; x < maxX; ++x)
          for (auto i = 0; i < sampler.getSampleCount(); ++i)
            film.expose({x, y},
                        results[sampler.getSampleCount() *
                                    ((maxX - minX) * (y - minY) + (x - minX)) +
                                i]);
    }
  }

  auto image = film.develop();

  std::ofstream os{"out.ppm", std::ios::binary};
  os << "P6 " << image.getSize().x << " " << image.getSize().y << " 255 ";

  for (auto y = image.getSize().y - 1; y >= 0; --y) {
    for (auto x = 0; x < image.getSize().x; ++x) {
      glm::vec3 color{image.load({x, y})};
      color = glm::pow(color, glm::vec3{1.0f / 2.2f});

      uint8_t r = 255.99f * color.r;
      uint8_t g = 255.99f * color.g;
      uint8_t b = 255.99f * color.b;
      os << r << g << b;
    }
  }

  return 0;
}