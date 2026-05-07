#pragma once

#include <Magnum/GL/Mesh.h>
#include <Magnum/SceneGraph/Object.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/ImGuiIntegration/Context.hpp>

#include <FGFDMExec.h>
#include <models/FGFCS.h>
#include <models/FGPropulsion.h>

#include <memory>

#include "model/model.h"
#include "model/textureddrawable.h"
#include "utils/utils.h"
#include "types/types.h"


class AircraftHandle {
  public:
    const types::AircraftType _aircraft_type;
    const std::string _aircraft_type_string;

    std::unordered_map<types::AircraftKeyPoints, types::Object3D *> _keypoints_mapping;

    // JSBSim
    std::unique_ptr<JSBSim::FGFDMExec> _fdmexec;
    std::shared_ptr<JSBSim::FGInitialCondition> _ic;
    
    // Magnum
    types::Object3D *_visual_root_object;
    std::vector<types::Object3DRenderable> _rendered_objects;

    explicit AircraftHandle(types::AircraftType aircraft_type)
      : _aircraft_type{aircraft_type}, _aircraft_type_string{utils::to_type_string(aircraft_type)} {}

    AircraftHandle& with_fdmexec(bool quiet = true);
    AircraftHandle& with_ic(types::AircraftInitialConditionPreset preset);
    AircraftHandle& with_visual_root(types::Object3D *object);
    AircraftHandle& with_model(model::ModelRepository& model_repo);
    AircraftHandle& link(Magnum::Shaders::PhongGL& shader, Magnum::SceneGraph::DrawableGroup3D& drawables);

    types::AircraftStateInfo to_aircraft_state();

    void update_sim();
    void update_vis();
};
