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
