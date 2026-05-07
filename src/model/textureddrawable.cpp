#include "textureddrawable.h"

namespace model {

void TexturedDrawable::draw(const Magnum::Matrix4& transformation_matrix, Magnum::SceneGraph::Camera3D& camera) {
  _shader.setTransformationMatrix(transformation_matrix)
    .setProjectionMatrix(camera.projectionMatrix())
    .setNormalMatrix(transformation_matrix.normalMatrix());
  _shader.bindDiffuseTexture(*_texture);
  _mesh.draw(_shader);
}

};
