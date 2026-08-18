#include "aircraft_control_component.h"

void AircraftControlComponent::init() {}

void AircraftControlComponent::quit() {}

/// Move aircraft depending on human input
void AircraftControlComponent::handle_dispatch() {
  // Check for whether there's an active aircraft, and whether we are in the correct control mode
  if (blackboard->sim_state_blackboard->has_active_aircraft != types::eAircraftActive::HAS_ACTIVE ||
      blackboard->sim_state_blackboard->sim_control_type != types::eSimControlType::MODEL) {
    return;
  }
  
  // Only move active aircraft
  const size_t active_aircraft_index = blackboard->aircraft_blackboard->active_aircraft_index;
  const AircraftHandle &active_aircraft = blackboard->aircraft_blackboard->aircraft[active_aircraft_index];
  std::shared_ptr<JSBSim::FGFCS> fcs = active_aircraft._fdmexec->GetFCS();

  // Yaw/Pitch/Roll
  fcs->SetDrCmd( blackboard->input_blackboard->commanded_aircraft_yaw);
  fcs->SetDeCmd(-blackboard->input_blackboard->commanded_aircraft_pitch);
  fcs->SetDaCmd(-blackboard->input_blackboard->commanded_aircraft_roll);
  
  // Steering
  fcs->SetDsCmd(-blackboard->input_blackboard->commanded_aircraft_yaw);

  // Throttle (persistent)
  const double cur_throttle_command = blackboard->input_blackboard->commanded_aircraft_throttle;
  const double prev_throttle_command = fcs->GetThrottleCmd(0);
  double next_throttle_command = prev_throttle_command + cur_throttle_command;
  if (next_throttle_command < 0.0) next_throttle_command = 0.0;
  fcs->SetThrottleCmd(0, next_throttle_command);
  
  // Braking
  float braking = blackboard->input_blackboard->commanded_aircraft_braking;
  if (braking < 0.0) braking = 0.0;
  fcs->SetDsbCmd(braking);
  fcs->SetCBrake(braking);
  fcs->SetLBrake(braking);
  fcs->SetRBrake(braking);
}
