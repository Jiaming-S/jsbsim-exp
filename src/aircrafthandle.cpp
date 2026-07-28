#include "aircrafthandle.h"
#include "drawn/traildrawable.h"
#include "initialization/FGTrim.h"


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

AircraftHandle& AircraftHandle::with_keypoints(std::vector<types::AircraftKeyPoints> keypoints) {
  assert(_visual_root_object);

  for (auto& keypoint : keypoints) {
    Magnum::Vector3 keypoint_relative_coords = utils::to_keypoint_coords(_aircraft_type, keypoint);
    types::Object3D *keypoint_object = new types::Object3D{_visual_root_object};
    keypoint_object->translate(keypoint_relative_coords);
    _aircraft_keypoints_mapping[keypoint] = keypoint_object;
  }

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

    _rendered_objects.push_back(types::Object3DRenderable{
      part_node,
      mesh_ptr,
      texture_ptr
    });
  }

  return *this;
}

AircraftHandle& AircraftHandle::link(
  Magnum::Shaders::PhongGL& shader,
  Magnum::SceneGraph::DrawableGroup3D& drawables
) {
  for (auto& model_part : _rendered_objects) {
    new drawn::TexturedDrawable{
      *model_part.node,
      shader,
      *model_part.mesh,
      model_part.texture,
      drawables
    };
  }

  return *this;
}

AircraftHandle& AircraftHandle::link_trails(
  types::Object3D& object,
  Magnum::SceneGraph::DrawableGroup3D& drawables
) {
  new drawn::TrailDrawable{
    object,
    drawables,
    _aircraft_trail,
    _aircraft_type,
    types::AircraftKeyPoints::WINGTIP_L,
    {1.0f, 1.0f, 1.0f}
  };

  new drawn::TrailDrawable{
    object,
    drawables,
    _aircraft_trail,
    _aircraft_type,
    types::AircraftKeyPoints::WINGTIP_R,
    {1.0f, 1.0f, 1.0f}
  };

  return *this;
}

AircraftHandle& AircraftHandle::link_shadow(
  shaders::ShadowShader& shader,
  Magnum::SceneGraph::DrawableGroup3D& drawables
) {
  assert(_visual_root_object);
  new drawn::ShadowDrawable{*_visual_root_object, shader, drawables};
  return *this;
}

types::AircraftStateInfo AircraftHandle::as_aircraft_state() const {
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
