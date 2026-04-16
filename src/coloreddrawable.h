#pragma once

#include <Magnum/Math/Color.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/Shaders/PhongGL.h>

#include "types/types.h"

using namespace Magnum::Math::Literals;


class ColoredDrawable : public Magnum::SceneGraph::Drawable3D {
  Magnum::Shaders::PhongGL& _shader;
  Magnum::GL::Mesh& _mesh;

  public:
    explicit ColoredDrawable(types::Object3D& object, Magnum::Shaders::PhongGL& shader, Magnum::GL::Mesh& mesh, Magnum::SceneGraph::DrawableGroup3D& group)
      : Magnum::SceneGraph::Drawable3D{object, &group}, _shader(shader), _mesh(mesh) {}

  private:
    void draw(const Magnum::Matrix4& transformation_maxtrix, Magnum::SceneGraph::Camera3D& camera) override;
};


