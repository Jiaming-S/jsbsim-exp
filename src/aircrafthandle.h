#pragma once

#include <Corrade/Containers/ArrayViewStl.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/SceneGraph/Object.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/ImGuiIntegration/Context.hpp>

#include <FGFDMExec.h>
#include <models/FGFCS.h>
#include <models/FGPropulsion.h>

#include <memory>
#include <deque>

#include "drawn/textureddrawable.h"
#include "drawn/traildrawable.h"
#include "drawn/shadowdrawable.h"
#include "model/model.h"
#include "utils/utils.h"
#include "types/types.h"


class AircraftHandle {
  public:
    const types::AircraftType _aircraft_type;
    const std::string _aircraft_type_string;

    // Aircraft keypoints
    std::unordered_map<types::AircraftKeyPoints, types::Object3D *> _aircraft_keypoints_mapping;
    std::shared_ptr<std::deque<types::AircraftTrailBreadcrumb>> _aircraft_trail;
    const size_t _aircraft_trail_size_limit = 480;

    // JSBSim
    std::unique_ptr<JSBSim::FGFDMExec> _fdmexec;
    std::shared_ptr<JSBSim::FGInitialCondition> _ic;
    
    // Magnum
    types::Object3D *_visual_root_object;
    std::vector<types::Object3DRenderable> _rendered_objects;

    explicit AircraftHandle(types::AircraftType aircraft_type)
      : _aircraft_type{aircraft_type}, _aircraft_type_string{utils::to_type_string(aircraft_type)},
        _aircraft_trail{std::make_shared<std::deque<types::AircraftTrailBreadcrumb>>()} {}

    AircraftHandle& with_fdmexec(bool quiet = true);
    AircraftHandle& with_ic(types::AircraftInitialConditionPreset preset);
    AircraftHandle& with_visual_root(types::Object3D *object);
    AircraftHandle& with_keypoints(std::vector<types::AircraftKeyPoints> keypoints);
    AircraftHandle& with_model(std::shared_ptr<model::ModelMultipartTextured> model);
    AircraftHandle& link(Magnum::Shaders::PhongGL& shader, Magnum::SceneGraph::DrawableGroup3D& drawables);
    AircraftHandle& link_trails(types::Object3D& object, Magnum::SceneGraph::DrawableGroup3D& drawables);
    AircraftHandle& link_shadow(shaders::ShadowShader& shader, Magnum::SceneGraph::DrawableGroup3D& drawables);
    
    /// @brief Outputs an `AircraftStateInfo` representation from the AircraftHandle's
    ///        hooked JSBSim FGFDMExec
    types::AircraftStateInfo as_aircraft_state();
};
