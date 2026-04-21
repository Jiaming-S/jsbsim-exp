#pragma once

#include <Magnum/GL/Mesh.h>
#include <Magnum/SceneGraph/Object.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/ImGuiIntegration/Context.hpp>

#include <FGFDMExec.h>

#include <memory>

#include "coloreddrawable.h"
#include "utils/utils.h"
#include "types/types.h"


struct _NodeMeshPair {
  types::Object3D *node;
  Magnum::GL::Mesh *mesh;
};

struct _AircraftStateInfo {
  Magnum::Rad pitch;
  Magnum::Rad roll;
  Magnum::Rad yaw;
  double alt;
  double north;
  double east;
  double down;
  double v_north;
  double v_east;
  double v_down;
};

class AircraftHandle {
  public:
    const types::AircraftType _aircraft_type;
    const std::string _aircraft_type_string;

    // JSBSim
    std::unique_ptr<JSBSim::FGFDMExec> _fdmexec;
    std::shared_ptr<JSBSim::FGInitialCondition> _ic;
    
    // Magnum
    types::Object3D *_model;
    std::vector<_NodeMeshPair> _model_parts;

    explicit AircraftHandle(types::AircraftType aircraft_type)
      : _aircraft_type{aircraft_type}, _aircraft_type_string{utils::to_type_string(aircraft_type)} {}

    AircraftHandle& with_fdmexec(bool quiet = true);
    AircraftHandle& with_ic(types::AircraftInitialConditionConfig config, bool quiet = true);
    AircraftHandle& with_model(types::Object3D *model);
    AircraftHandle& with_meshes(std::unordered_map<std::string, std::vector<types::ModelPart>>& meshes);
    AircraftHandle& link(Magnum::Shaders::PhongGL& shader, Magnum::SceneGraph::DrawableGroup3D& drawables);

    _AircraftStateInfo to_aircraft_state();

    void update_sim();
    void update_model();
};
