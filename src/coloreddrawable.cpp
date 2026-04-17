#include "coloreddrawable.h"

void ColoredDrawable::draw(const Magnum::Matrix4& transformation_maxtrix, Magnum::SceneGraph::Camera3D& camera) {
  // Apply camera and object transformations to the shader
  _shader.setLightPositions({{7.0f, 5.0f, 2.5f, 0.0f}})
    // .setDiffuseColor(0x3bd267_rgbf)
    .setTransformationMatrix(transformation_maxtrix)
    .setNormalMatrix(transformation_maxtrix.normalMatrix())
    .setProjectionMatrix(camera.projectionMatrix());
  _mesh.draw(_shader);
}
