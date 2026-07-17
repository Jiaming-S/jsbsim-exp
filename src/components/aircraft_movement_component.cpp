#include "aircraft_movement_component.h"

void AircraftMovementComponent::init() {}

void AircraftMovementComponent::quit() {}

void AircraftMovementComponent::handle_dispatch() {
  // Move all aircraft depending on input blackboard commands
  for (auto &ac : blackboard->aircraft_blackboard_vec->aircraft) {
    std::shared_ptr<JSBSim::FGFCS> fcs = ac._fdmexec->GetFCS();

    // Yaw/Pitch/Roll
    fcs->SetDrCmd( blackboard->input_blackboard->commanded_yaw);
    fcs->SetDeCmd(-blackboard->input_blackboard->commanded_pitch);
    fcs->SetDaCmd(-blackboard->input_blackboard->commanded_roll);
    
    // Steering
    fcs->SetDsCmd(-blackboard->input_blackboard->commanded_yaw);

    // Engine (positive throttle)
    float throttle = blackboard->input_blackboard->commanded_translation.z();
    if (throttle < 0.0) throttle = 0.0;
    fcs->SetThrottleCmd(0, throttle);
    
    // Braking (negative throttle)
    float braking = blackboard->input_blackboard->commanded_translation.z();
    if (braking > 0.0) throttle = 0.0;
    fcs->SetDsbCmd(braking);
    fcs->SetCBrake(braking);
    fcs->SetLBrake(braking);
    fcs->SetRBrake(braking);
  }
}
