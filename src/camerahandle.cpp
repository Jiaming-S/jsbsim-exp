#include "camerahandle.h"

CameraHandle::CameraHandle(types::Object3D* root, Magnum::Matrix4 projection_matrix) {
  _mount = new types::Object3D{root};
  _revolut = new types::Object3D{_mount};
  _camera = new Magnum::SceneGraph::Camera3D{*_revolut};
  _camera->setProjectionMatrix(projection_matrix);
}

void CameraHandle::reattach_to(types::Object3D* root) {
  _mount->setParent(root);
  _mount->setTransformation(Magnum::Matrix4{});
  _revolut->setTransformation(Magnum::Matrix4{});
}

void CameraHandle::apply_commanded_movement(
  input::CommandedMovement& commanded_movement,
  types::SimContext& sim_context
) {
  if (sim_context.camera_type == types::SimContext::CameraType::FREE) {
    Magnum::Matrix4 yaw_mat = Magnum::Matrix4::rotation(Magnum::Deg(commanded_movement.yaw), Magnum::Vector3::yAxis());
    _revolut->setTransformation(yaw_mat * _revolut->transformation());
  }
  else {
    _revolut->rotateLocal(Magnum::Deg(commanded_movement.yaw),   Magnum::Vector3::yAxis());
  }

  _revolut->rotateLocal(Magnum::Deg(commanded_movement.pitch), Magnum::Vector3::xAxis());
  _revolut->rotateLocal(Magnum::Deg(commanded_movement.roll),  Magnum::Vector3::zAxis());

  Magnum::Vector3 right = _revolut->transformation().right();
  Magnum::Vector3 up    = _revolut->transformation().up();
  Magnum::Vector3 back  = _revolut->transformation().backward();

  _mount->translate(
    right * commanded_movement.translation.x() +
    up    * commanded_movement.translation.y() +
    back  * commanded_movement.translation.z()
  );
}
