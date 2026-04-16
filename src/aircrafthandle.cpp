#include "aircrafthandle.h"


AircraftHandle& AircraftHandle::with_fdmexec(bool quiet) {
  assert(_aircraft_type_string.size());

  _fdmexec = utils::load_aircraft(_aircraft_type, _aircraft_type_string, quiet);
  _ic = _fdmexec->GetIC();

  return *this;
}

AircraftHandle& AircraftHandle::with_ic(types::AircraftInitialConditionConfig& config, bool quiet) {
  assert(_ic);

  utils::load_aircraft_ic_config(_ic, config);
  _fdmexec->RunIC();

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

