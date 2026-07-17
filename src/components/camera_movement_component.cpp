#include "camera_movement_component.h"

void CameraMovementComponent::init() {}

void CameraMovementComponent::quit() {}

void CameraMovementComponent::handle_dispatch() {
  CameraHandle &active_camera = blackboard->camera_blackboard_vec->cameras[blackboard->camera_blackboard_vec->active_camera_index];

  if (blackboard->sim_state_blackboard->camera_type == types::eCameraType::FREE) {
    Magnum::Matrix4 yaw_mat = Magnum::Matrix4::rotation(Magnum::Deg(blackboard->input_blackboard->commanded_yaw), Magnum::Vector3::yAxis());
    active_camera._revolut->setTransformation(yaw_mat * active_camera._revolut->transformation());
  }
  else if (blackboard->sim_state_blackboard->camera_type == types::eCameraType::LOCKED) {
    active_camera._revolut->rotateLocal(Magnum::Deg(blackboard->input_blackboard->commanded_yaw), Magnum::Vector3::yAxis());
  }

  active_camera._revolut->rotateLocal(Magnum::Deg(blackboard->input_blackboard->commanded_pitch), Magnum::Vector3::xAxis());
  active_camera._revolut->rotateLocal(Magnum::Deg(blackboard->input_blackboard->commanded_roll),  Magnum::Vector3::zAxis());

  Magnum::Vector3 right = active_camera._revolut->transformation().right();
  Magnum::Vector3 up    = active_camera._revolut->transformation().up();
  Magnum::Vector3 back  = active_camera._revolut->transformation().backward();

  active_camera._mount->translate(
    right * blackboard->input_blackboard->commanded_translation.x() +
    up    * blackboard->input_blackboard->commanded_translation.y() +
    back  * blackboard->input_blackboard->commanded_translation.z()
  );
}