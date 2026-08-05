#include "aircraft_control_component.h"

void AircraftControlComponent::init() {}

void AircraftControlComponent::quit() {}

/// Move aircraft depending on human input
void AircraftControlComponent::handle_dispatch() {
  // Check for whether there's an active aircraft, and whether we are in the correct control mode
  if (!blackboard->sim_state_blackboard->has_active_aircraft ||
      blackboard->sim_state_blackboard->sim_control_type != types::eSimControlType::MODEL) {
    return;
  }
  
  // Only move active aircraft
  const size_t active_aircraft_index = blackboard->aircraft_blackboard->active_aircraft_index;
  const AircraftHandle &active_aircraft = blackboard->aircraft_blackboard->aircraft[active_aircraft_index];
  std::shared_ptr<JSBSim::FGFCS> fcs = active_aircraft._fdmexec->GetFCS();

  // Yaw/Pitch/Roll
  fcs->SetDrCmd( blackboard->input_blackboard->commanded_yaw);
  fcs->SetDeCmd(-blackboard->input_blackboard->commanded_pitch);
  fcs->SetDaCmd(-blackboard->input_blackboard->commanded_roll);
  
  // Steering
  fcs->SetDsCmd(-blackboard->input_blackboard->commanded_yaw);

  // Engine (positive throttle, retained)
  double throttle_command = -blackboard->input_blackboard->commanded_translation.z();
  double prev_throttle_pos = fcs->GetThrottlePos(0);
  double next_throttle_pos = prev_throttle_pos + throttle_command * 0.1;
  fcs->SetThrottleCmd(0, next_throttle_pos);
  
  // Braking (negative throttle, momentary)
  float braking = -throttle_command;
  if (braking > 0.0) braking = 0.0;
  fcs->SetDsbCmd(braking);
  fcs->SetCBrake(braking);
  fcs->SetLBrake(braking);
  fcs->SetRBrake(braking);
}
