#include "aircraft_state_info_component.h"
#include "Magnum/Magnum.h"

void AircraftStateInfoComponent::init() {}

void AircraftStateInfoComponent::quit() {}

void AircraftStateInfoComponent::handle_dispatch() {
  // Update all aircraft state infos
  for (auto &ac : blackboard->aircraft_blackboard->aircraft) {
    std::shared_ptr<JSBSim::FGAerodynamics> cur_aerodynamics = ac._fdmexec->GetAerodynamics();
    std::shared_ptr<JSBSim::FGPropagate> cur_propagate = ac._fdmexec->GetPropagate();

    Magnum::Rad roll  = Magnum::Rad(cur_propagate->GetEuler(1));
    Magnum::Rad pitch = Magnum::Rad(cur_propagate->GetEuler(2));
    Magnum::Rad yaw   = Magnum::Rad(cur_propagate->GetEuler(3));

    Magnum::Rad roll_deg  = Magnum::Deg(roll);
    Magnum::Rad pitch_deg = Magnum::Deg(pitch);
    Magnum::Rad yaw_deg   = Magnum::Deg(yaw);

    Magnum::Rad alpha = Magnum::Rad(cur_aerodynamics->GetAlphaW());
    Magnum::Rad alpha_deg = Magnum::Deg(alpha);

    double alt = cur_propagate->GetAltitudeASL();
    double radius  = cur_propagate->GetRadius();

    double lat_rad = cur_propagate->GetLatitude();
    double lon_rad = cur_propagate->GetLongitude();

    double north = lat_rad * radius;
    double east  = lon_rad * radius * std::cos(cur_propagate->GetLatitude());
    double down  = -alt;

    double v_north = cur_propagate->GetVel(1);
    double v_east  = cur_propagate->GetVel(2);
    double v_down  = cur_propagate->GetVel(3);

    double s_ground = std::sqrt(pow(v_north, 2) + pow(v_east, 2));

    ac._state_info = types::AircraftStateInfo {
      .pitch = pitch,
      .roll = roll,
      .yaw = yaw,
      .pitch_deg = pitch_deg,
      .roll_deg = roll_deg,
      .yaw_deg = yaw_deg,
      .alpha = alpha,
      .alpha_deg = alpha_deg,
      .alt = alt,
      .north = north,
      .east = east,
      .down = down,
      .v_north = v_north,
      .v_east = v_east,
      .v_down = v_down,
      .s_ground = s_ground,
    };
  }
}
