#include "aircrafthandle.h"


AircraftHandle& AircraftHandle::with_fdmexec(bool quiet) {
  assert(_aircraft_type_string.size());

  _fdmexec = utils::load_aircraft(_aircraft_type, _aircraft_type_string, quiet);
  _ic = _fdmexec->GetIC();

  return *this;
}

AircraftHandle& AircraftHandle::with_ic(types::AircraftInitialConditionPreset preset) {
  assert(_fdmexec);

  JSBSim::FGFDMExec *fdmexec_raw_ptr = _fdmexec.get();
  
  // Apply initial condition presets
  _fdmexec->GetIC()->InitializeIC();
  utils::apply_preset_ic(preset, *fdmexec_raw_ptr);
  _fdmexec->RunIC();
  
  // Apply control and throttle presets
  utils::apply_preset_controls(preset, *fdmexec_raw_ptr);

  return *this;
}

AircraftHandle& AircraftHandle::with_model(types::Object3D *model) {
  _model = model;
  return *this;
}

AircraftHandle& AircraftHandle::with_meshes(std::unordered_map<std::string, std::vector<types::ModelPart>>& meshes) {
  assert(_aircraft_type_string.size());
  assert(meshes[_aircraft_type_string].size());

  for (auto& part : meshes[_aircraft_type_string]) {
    types::Object3D* part_node = new types::Object3D{_model};
    part_node->setTransformation(part.transformation);
    _model_parts.push_back(_NodeMeshPair{
      part_node,
      &(part.mesh)
    }); 
  }

  return *this;
}

AircraftHandle& AircraftHandle::link(Magnum::Shaders::PhongGL& shader, Magnum::SceneGraph::DrawableGroup3D& drawables) {
  for (auto& model_part : _model_parts) {
    new ColoredDrawable{
      *model_part.node,
      shader,
      *model_part.mesh,
      drawables
    };
  }

  return *this;
}

void AircraftHandle::update_sim() {
  _fdmexec->Run();
}

void AircraftHandle::update_model() {
  types::AircraftStateInfo state = this->to_aircraft_state();
  _model->resetTransformation()
    .rotateY(-state.yaw)
    .rotateX(-state.pitch)
    .rotateZ( state.roll)
    .translate(utils::as_magnum_RUB(state.north, state.east, state.down));
}

types::AircraftStateInfo AircraftHandle::to_aircraft_state() {
  std::shared_ptr<JSBSim::FGAircraft>  cur_aircraft  = _fdmexec->GetAircraft();
  std::shared_ptr<JSBSim::FGPropagate> cur_propagate = _fdmexec->GetPropagate();

  Magnum::Rad roll  = Magnum::Rad(cur_propagate->GetEuler(1));
  Magnum::Rad pitch = Magnum::Rad(cur_propagate->GetEuler(2));
  Magnum::Rad yaw   = Magnum::Rad(cur_propagate->GetEuler(3));

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

  return types::AircraftStateInfo {
    .pitch = pitch,
    .roll = roll,
    .yaw = yaw,
    .alt = alt,
    .north = north,
    .east = east,
    .down = down,
    .v_north = v_north,
    .v_east = v_east,
    .v_down = v_down,
  };
}
