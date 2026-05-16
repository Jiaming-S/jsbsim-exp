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

AircraftHandle& AircraftHandle::with_visual_root(types::Object3D *object) {
  _visual_root_object = object;
  return *this;
}

AircraftHandle& AircraftHandle::with_model(std::shared_ptr<model::ModelMultipartTextured> model) {
  assert(_visual_root_object);
  assert(model);

  for (auto& component : model->_components) {
    types::Object3D* part_node = new types::Object3D{_visual_root_object};
    part_node->setTransformation(component.local_transformation);
    Magnum::GL::Mesh* mesh_ptr = &model->_meshes[component.mesh_idx];
    Magnum::GL::Texture2D* texture_ptr = &model->_textures[component.texture_idx];

    _rendered_objects.push_back(types::Object3DRenderable {
      part_node,
      mesh_ptr,
      texture_ptr
    });
  }

  return *this;
}

AircraftHandle& AircraftHandle::link(Magnum::Shaders::PhongGL& shader, Magnum::SceneGraph::DrawableGroup3D& drawables) {
  for (auto& model_part : _rendered_objects) {
    new model::TexturedDrawable {
      *model_part.node,
      shader,
      *model_part.mesh,
      model_part.texture,
      drawables
    };
  }

  return *this;
}

void AircraftHandle::apply_commanded_movement(input::CommandedMovement commanded_movement) {
  std::shared_ptr<JSBSim::FGFCS> fcs = _fdmexec->GetFCS();

  // Yaw/Pitch/Roll
  fcs->SetDrCmd( commanded_movement.yaw);
  fcs->SetDeCmd(-commanded_movement.pitch);
  fcs->SetDaCmd(-commanded_movement.roll);
  
  // Steering
  fcs->SetDsCmd(-commanded_movement.yaw);

  // Engine (positive throttle)
  float throttle = commanded_movement.z < 0 ? -commanded_movement.z : 0;
  fcs->SetThrottleCmd(0, throttle);
  
  // Braking (negative throttle)
  float braking = commanded_movement.z > 0 ? commanded_movement.z : 0;
  fcs->SetDsbCmd(braking);
  fcs->SetCBrake(braking);
  fcs->SetLBrake(braking);
  fcs->SetRBrake(braking);
}

void AircraftHandle::update_sim() {
  _fdmexec->Run();
}

void AircraftHandle::update_vis() {
  types::AircraftStateInfo state = this->to_aircraft_state();
  _visual_root_object->resetTransformation()
    .rotateZ(-state.roll)
    .rotateX( state.pitch)
    .rotateY(-state.yaw)
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
