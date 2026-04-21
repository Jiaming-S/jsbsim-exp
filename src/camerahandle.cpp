#include "camerahandle.h"

void CameraHandle::attach_to(types::Object3D* root, Magnum::Matrix4 projection_matrix) {
  _mount = new types::Object3D{root};
  _revolut = new types::Object3D{_mount};
  _camera = new Magnum::SceneGraph::Camera3D{*_revolut};
  _camera->setProjectionMatrix(projection_matrix);
}

void CameraHandle::handle_keypress(std::unordered_map<Sdl2Application::Key, bool>& keys_down) {
  float speed = _default_speed;
  if (keys_down[Sdl2Application::Key::LeftShift]) speed *= SHIFT_SPEED_MULTIPLIER;

  if (keys_down[Sdl2Application::Key::Up])   _revolut->rotateLocal( _default_rotation_speed, Magnum::Vector3::xAxis());
  if (keys_down[Sdl2Application::Key::Down]) _revolut->rotateLocal(-_default_rotation_speed, Magnum::Vector3::xAxis());
  
  if (keys_down[Sdl2Application::Key::Left]) {
    Magnum::Matrix4 rot = Magnum::Matrix4::rotation(_default_rotation_speed, Magnum::Vector3::yAxis());
    _revolut->setTransformation(rot * _revolut->transformation());
  }

  if (keys_down[Sdl2Application::Key::Right]) {
    Magnum::Matrix4 rot = Magnum::Matrix4::rotation(-_default_rotation_speed, Magnum::Vector3::yAxis());
    _revolut->setTransformation(rot * _revolut->transformation());
  }

  Magnum::Vector3 forward = _revolut->transformation().backward();
  Magnum::Vector3 right   = _revolut->transformation().right();

  if (keys_down[Sdl2Application::Key::W]) _mount->translate(forward * -speed);
  if (keys_down[Sdl2Application::Key::S]) _mount->translate(forward *  speed);
  if (keys_down[Sdl2Application::Key::A]) _mount->translate(right * -speed);
  if (keys_down[Sdl2Application::Key::D]) _mount->translate(right *  speed);

  if (keys_down[Sdl2Application::Key::Space])     _mount->translate(Magnum::Vector3::yAxis( speed));
  if (keys_down[Sdl2Application::Key::LeftCtrl])  _mount->translate(Magnum::Vector3::yAxis(-speed));
}
