#pragma once

#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/Math/Color.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/Shaders/FlatGL.h>

#include "../types/types.h"
#include "../camerahandle.h"

namespace drawn {

class SkyboxDrawable: public Magnum::SceneGraph::Drawable3D {
  Magnum::Shaders::FlatGL3D& _shader;
  CameraHandle& _cam;
  Magnum::GL::Mesh _mesh;

  public:
    explicit SkyboxDrawable(
      types::Object3D& object,
      Magnum::Shaders::FlatGL3D& shader,
      CameraHandle& cam,
      Magnum::SceneGraph::DrawableGroup3D& group
    ) : Magnum::SceneGraph::Drawable3D{object, &group}, _shader(shader), _cam(cam) {
      
      // Initialize mesh for a fat plus
      float radius = 48.0f;
      float width = 2.0f;

      // All vertices
      Magnum::Vector3 vertices[] = {
        {-radius, 0.0f, -width},  { radius, 0.0f, -width},
        {-radius, 0.0f,  width},  { radius, 0.0f,  width},
        {-width,  0.0f, -radius}, { width,  0.0f, -radius},
        {-width,  0.0f,  radius}, { width,  0.0f,  radius}
      };

      // Index mappings
      Magnum::UnsignedInt indices[] = {
        0, 2, 1,   1, 2, 3, // Horizontal bar
        4, 6, 5,   5, 6, 7  // Vertical bar
      };

      _mesh.setPrimitive(Magnum::GL::MeshPrimitive::Triangles)
        .setCount(12)
        .addVertexBuffer(Magnum::GL::Buffer{vertices}, 0, Magnum::Shaders::FlatGL3D::Position{})
        .setIndexBuffer(Magnum::GL::Buffer{indices}, 0, Magnum::GL::MeshIndexType::UnsignedInt);
    }

  private:
    void draw(
      const Magnum::Matrix4& transformation_matrix,
      Magnum::SceneGraph::Camera3D& camera
    ) override {
      Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::Blending);
      
      _shader.setTransformationProjectionMatrix(camera.projectionMatrix() * transformation_matrix)
        .setColor(Magnum::Color4{{1.0f, 1.0f, 1.0f, 0.5f}});
      _mesh.draw(_shader);
      
      Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::Blending);
    }
};

};




