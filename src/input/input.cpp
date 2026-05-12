#include "input.h"

namespace input {

void GlobalInputHandler::handle_key_press_event(Magnum::Platform::Sdl2Application::KeyEvent& event) {
  _keys_down[event.key()] = true;
}

void GlobalInputHandler::handle_key_release_event(Magnum::Platform::Sdl2Application::KeyEvent& event) {
  _keys_down[event.key()] = false;
}

void GlobalInputHandler::handle_pointer_press_event(Magnum::Platform::Sdl2Application::PointerEvent& event) {
  _mouse_held = true;
  _mouse_cur_position = event.position(); 
  _mouse_prev_position = _mouse_cur_position;
}

void GlobalInputHandler::handle_pointer_release_event(Magnum::Platform::Sdl2Application::PointerEvent& event) {
  _mouse_held = false;
}

void GlobalInputHandler::handle_pointer_move_event(Magnum::Platform::Sdl2Application::PointerMoveEvent& event) {
  _mouse_cur_position = event.position();
}

void GlobalInputHandler::perform_camera_move(
  CameraHandle& cam,
  float mouse_sensitivity,
  float default_translation_speed,
  Magnum::Deg default_rotation_speed,
  float shift_translation_speed_multiplier,
  float shift_rotation_speed_multiplier
) {
  float translation_speed = default_translation_speed;
  Magnum::Deg rotation_speed = default_rotation_speed;

  // KEYBOARD CONTROLS
  if (_keys_down[Sdl2Application::Key::LeftShift]) translation_speed *= shift_translation_speed_multiplier;
  if (_keys_down[Sdl2Application::Key::LeftShift]) rotation_speed    *= shift_rotation_speed_multiplier;

  if (_keys_down[Sdl2Application::Key::Up])   cam._revolut->rotateLocal( rotation_speed, Magnum::Vector3::xAxis());
  if (_keys_down[Sdl2Application::Key::Down]) cam._revolut->rotateLocal(-rotation_speed, Magnum::Vector3::xAxis());
  
  if (_keys_down[Sdl2Application::Key::Left]) {
    Magnum::Matrix4 rot = Magnum::Matrix4::rotation(rotation_speed, Magnum::Vector3::yAxis());
    cam._revolut->setTransformation(rot * cam._revolut->transformation());
  }

  if (_keys_down[Sdl2Application::Key::Right]) {
    Magnum::Matrix4 rot = Magnum::Matrix4::rotation(-rotation_speed, Magnum::Vector3::yAxis());
    cam._revolut->setTransformation(rot * cam._revolut->transformation());
  }

  Magnum::Vector3 forward = cam._revolut->transformation().backward();
  Magnum::Vector3 right   = cam._revolut->transformation().right();

  if (_keys_down[Sdl2Application::Key::W]) cam._mount->translate(forward * -translation_speed);
  if (_keys_down[Sdl2Application::Key::S]) cam._mount->translate(forward *  translation_speed);
  if (_keys_down[Sdl2Application::Key::A]) cam._mount->translate(right * -translation_speed);
  if (_keys_down[Sdl2Application::Key::D]) cam._mount->translate(right *  translation_speed);

  if (_keys_down[Sdl2Application::Key::Space])     cam._mount->translate(Magnum::Vector3::yAxis( translation_speed));
  if (_keys_down[Sdl2Application::Key::LeftCtrl])  cam._mount->translate(Magnum::Vector3::yAxis(-translation_speed));

  // MOUSE CONTROLS
  if (_mouse_held) {
    Magnum::Vector2 delta_pos = Magnum::Vector2(_mouse_cur_position - _mouse_prev_position) * _mouse_sensitivity;
    Magnum::Matrix4 delta_rot = Magnum::Matrix4::rotation(Magnum::Deg(-delta_pos.x()), Magnum::Vector3::yAxis());
    cam._revolut->setTransformation(delta_rot * cam._revolut->transformation());
    cam._revolut->rotateLocal(Magnum::Deg(-delta_pos.y()), Magnum::Vector3::xAxis());
    _mouse_prev_position = _mouse_cur_position; 
  }
}

void GlobalInputHandler::perform_object_move(types::Object3D *obj) {

}


}

