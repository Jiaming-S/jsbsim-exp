#include "coloreddrawable.h"

namespace model {

void ColoredDrawable::draw(const Magnum::Matrix4& transformation_maxtrix, Magnum::SceneGraph::Camera3D& camera) {
  _shader.setLightPositions({{7.0f, 5.0f, 2.5f, 0.0f}})
    .setTransformationMatrix(transformation_maxtrix)
    .setNormalMatrix(transformation_maxtrix.normalMatrix())
    .setProjectionMatrix(camera.projectionMatrix());
  _mesh.draw(_shader);
}

};
