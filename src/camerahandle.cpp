#include "camerahandle.h"

CameraHandle& CameraHandle::attach_to(types::Object3D* root) {
  _mount->setParent(root);
  _mount->setTransformation(Magnum::Matrix4{});
  _revolut->setTransformation(Magnum::Matrix4{});
  return *this;
}

CameraHandle& CameraHandle::attach_to_scene_root(types::Object3D* scene_root) {
  // Get world position and orientation of _revolut
  const Magnum::Matrix4 prev_world_position = _revolut->absoluteTransformation();
  const Magnum::Matrix4 prev_orientation = _revolut->transformation();

  // Attach _mount to scene root
  this->attach_to(scene_root);

  // Apply previous position and rotation
  _mount->translate(prev_world_position.translation());
  _revolut->transform(prev_orientation);
  return *this;
}

CameraHandle& CameraHandle::with_projection_matrix(Magnum::Matrix4 projection_matrix) {
  _camera->setProjectionMatrix(projection_matrix);
  return *this;
}

CameraHandle& CameraHandle::with_default_offset(types::eSimControlType control_type) {
  switch (control_type) {
    case types::eSimControlType::CAMERA: {
      _mount->translate(_camera->projectionMatrix().up() * 50);
      _mount->translate(_camera->projectionMatrix().backward() * -100);
      break;
    }
    case types::eSimControlType::MODEL: {
      _mount->translate(_camera->projectionMatrix().up() * 10);
      _mount->translate(_camera->projectionMatrix().backward() * -50);
      break;
    }
  }
  return *this;
}
