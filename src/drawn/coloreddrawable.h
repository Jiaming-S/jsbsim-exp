#pragma once

#include <Magnum/Math/Color.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/Shaders/PhongGL.h>

#include "../types/types.h"

namespace drawn {

class ColoredDrawable: public Magnum::SceneGraph::Drawable3D {
  Magnum::Shaders::PhongGL& _shader;
  Magnum::GL::Mesh& _mesh;

  public:
    explicit ColoredDrawable(
      types::Object3D& object,
      Magnum::Shaders::PhongGL& shader,
      Magnum::GL::Mesh& mesh,
      Magnum::SceneGraph::DrawableGroup3D& group
    ) : Magnum::SceneGraph::Drawable3D{object, &group}, _shader(shader), _mesh(mesh) {}

  private:
    void draw(
      const Magnum::Matrix4& transformation_matrix,
      Magnum::SceneGraph::Camera3D& camera
    ) override {
      // Transform world-space sun direction to camera space (view space)
      Magnum::Matrix4 view_matrix = camera.object().absoluteTransformationMatrix().inverted();
      Magnum::Vector4 light_dir_camera = view_matrix * Magnum::Vector4{0.3f, 0.5f, -0.6f, 0.0f};

      _shader.setTransformationMatrix(transformation_matrix)
        .setNormalMatrix(transformation_matrix.normalMatrix())
        .setProjectionMatrix(camera.projectionMatrix())
        .setLightPositions({light_dir_camera})
        .setAmbientColor(Magnum::Color4{0.35f, 0.35f, 0.35f, 1.0f})
        .setSpecularColor(Magnum::Color4{0.4f, 0.4f, 0.4f, 1.0f})
        .setShininess(60.0f);

      _shader.draw(_mesh);
    }
};

};

