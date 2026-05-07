#pragma once

#include <Magnum/Math/Color.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/Shaders/PhongGL.h>

#include "../types/types.h"

namespace model {

class TexturedDrawable: public Magnum::SceneGraph::Drawable3D {
  Magnum::Shaders::PhongGL& _shader;
  Magnum::GL::Mesh& _mesh;
  Magnum::GL::Texture2D* _texture;
  
  public:
    explicit TexturedDrawable(
      types::Object3D& object,
      Magnum::Shaders::PhongGL& shader,
      Magnum::GL::Mesh& mesh,
      Magnum::GL::Texture2D* texture,
      Magnum::SceneGraph::DrawableGroup3D& group
    ) : Magnum::SceneGraph::Drawable3D{object, &group}, _shader(shader), _mesh(mesh), _texture(texture) {}

  private:
    void draw(const Magnum::Matrix4& transformation_matrix, Magnum::SceneGraph::Camera3D& camera) override;
};

};

