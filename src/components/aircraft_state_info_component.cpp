#include "aircraft_state_info_component.h"

void AircraftStateInfoComponent::init() {}

void AircraftStateInfoComponent::quit() {}

void AircraftStateInfoComponent::handle_dispatch() {
  // Update all aircraft state infos
  for (auto &ac : blackboard->aircraft_blackboard->aircraft) {
    std::shared_ptr<JSBSim::FGAerodynamics> cur_aerodynamics = ac._fdmexec->GetAerodynamics();
    std::shared_ptr<JSBSim::FGPropagate> cur_propagate = ac._fdmexec->GetPropagate();

    Magnum::Rad roll_rad  = Magnum::Rad(cur_propagate->GetEuler(1));
    Magnum::Rad pitch_rad = Magnum::Rad(cur_propagate->GetEuler(2));
    Magnum::Rad yaw_rad   = Magnum::Rad(cur_propagate->GetEuler(3));
    Magnum::Rad roll_deg  = Magnum::Deg(roll_rad);
    Magnum::Rad pitch_deg = Magnum::Deg(pitch_rad);
    Magnum::Rad yaw_deg   = Magnum::Deg(yaw_rad);

    Magnum::Rad alpha_rad = Magnum::Rad(cur_aerodynamics->GetAlphaW());
    Magnum::Rad alpha_deg = Magnum::Deg(alpha_rad);

    double alt_ft = cur_propagate->GetAltitudeASL();
    double radius  = cur_propagate->GetRadius();

    double lat_rad = cur_propagate->GetLatitude();
    double lon_rad = cur_propagate->GetLongitude();

    double north_ft = lat_rad * radius;
    double east_ft  = lon_rad * radius * std::cos(cur_propagate->GetLatitude());
    double down_ft  = -alt_ft;

    double north_spd_fps = cur_propagate->GetVel(1);
    double east_spd_fps  = cur_propagate->GetVel(2);
    double down_spd_fps  = cur_propagate->GetVel(3);

    double ground_spd_fps = std::sqrt(pow(north_spd_fps, 2) + pow(east_spd_fps, 2));
    double indicated_spd_fps = std::sqrt(pow(north_spd_fps, 2) + pow(east_spd_fps, 2) + pow(down_spd_fps, 2));

    const double FEET_TO_KM = 0.0003048;
    const double FPS_TO_KPH = 1.09728;

    double alt_km = alt_ft * FEET_TO_KM;
    double north_km = north_ft * FEET_TO_KM;
    double east_km = east_ft * FEET_TO_KM;
    double down_km = down_ft * FEET_TO_KM;
    double north_spd_kph = north_spd_fps * FPS_TO_KPH;
    double east_spd_kph = east_spd_fps * FPS_TO_KPH;
    double down_spd_kph = down_spd_fps * FPS_TO_KPH;
    double ground_spd_kph = ground_spd_fps * FPS_TO_KPH;
    double indicated_spd_kph = indicated_spd_fps * FPS_TO_KPH;

    ac._state_info = types::AircraftStateInfo {
      .pitch_rad = pitch_rad,
      .roll_rad = roll_rad,
      .yaw_rad = yaw_rad,
      .pitch_deg = pitch_deg,
      .roll_deg = roll_deg,
      .yaw_deg = yaw_deg,
      .alpha_rad = alpha_rad,
      .alpha_deg = alpha_deg,
      .alt_ft = alt_ft,
      .north_ft = north_ft,
      .east_ft = east_ft,
      .down_ft = down_ft,
      .alt_km = alt_km,
      .north_km = north_km,
      .east_km = east_km,
      .down_km = down_km,
      .north_spd_fps = north_spd_fps,
      .east_spd_fps = east_spd_fps,
      .down_spd_fps = down_spd_fps,
      .ground_spd_fps = ground_spd_fps,
      .indicated_spd_fps = indicated_spd_fps,
      .north_spd_kph = north_spd_kph,
      .east_spd_kph = east_spd_kph,
      .down_spd_kph = down_spd_kph,
      .ground_spd_kph = ground_spd_kph,
      .indicated_spd_kph = indicated_spd_kph,
    };
  }
}
