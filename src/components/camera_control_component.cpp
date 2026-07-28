#include "camera_control_component.h"

void CameraControlComponent::init() {}

void CameraControlComponent::quit() {}

void CameraControlComponent::handle_dispatch() {
  if (blackboard->sim_state_blackboard->sim_control_type != types::eSimControlType::CAMERA) {
    return;
  }

  const double commanded_yaw = blackboard->keyboard_input_blackboard->commanded_yaw;
  const double commanded_pitch = blackboard->keyboard_input_blackboard->commanded_pitch;
  const double commanded_roll = blackboard->keyboard_input_blackboard->commanded_roll;

  const size_t active_camera_index = blackboard->camera_blackboard->active_camera_index;
  const CameraHandle &active_camera = blackboard->camera_blackboard->cameras[active_camera_index];

  if (blackboard->sim_state_blackboard->camera_type == types::eCameraType::FREE) {
    Magnum::Matrix4 yaw_mat = Magnum::Matrix4::rotation(Magnum::Deg(commanded_yaw), Magnum::Vector3::yAxis());
    active_camera._revolut->setTransformation(yaw_mat * active_camera._revolut->transformation());
  }
  else if (blackboard->sim_state_blackboard->camera_type == types::eCameraType::LOCKED) {
    active_camera._revolut->rotateLocal(Magnum::Deg(commanded_yaw), Magnum::Vector3::yAxis());
  }

  active_camera._revolut->rotateLocal(Magnum::Deg(commanded_pitch), Magnum::Vector3::xAxis());
  active_camera._revolut->rotateLocal(Magnum::Deg(commanded_roll),  Magnum::Vector3::zAxis());

  const Magnum::Vector3 right_direction = active_camera._revolut->transformation().right();
  const Magnum::Vector3 up_direction    = active_camera._revolut->transformation().up();
  const Magnum::Vector3 back_direction  = active_camera._revolut->transformation().backward();

  const float camera_speed = blackboard->camera_blackboard->camera_speed;
  const float commanded_right = camera_speed * blackboard->keyboard_input_blackboard->commanded_translation.x();
  const float commanded_up = camera_speed * blackboard->keyboard_input_blackboard->commanded_translation.y();
  const float commanded_back = camera_speed * blackboard->keyboard_input_blackboard->commanded_translation.z();

  active_camera._mount->translate(
    (right_direction * commanded_right) +
    (up_direction * commanded_up) +
    (back_direction * commanded_back)
  );
}
