#include "scenario_control_component.h"

void ScenarioControlComponent::init() {}

void ScenarioControlComponent::quit() {}

void perform_sim_reset() {

}

void ScenarioControlComponent::handle_dispatch() {
  const types::eScenarioResetRequest scenario_reset_request = blackboard->sim_state_blackboard->scenario_reset_request;

  // Populate preset
  std::vector<std::pair<types::AircraftType, types::AircraftInitialConditionPreset>> presets;
  switch (scenario_reset_request) {
    case types::eScenarioResetRequest::NO_REQUEST: {
      return;
    }
    case types::eScenarioResetRequest::REQUEST_RESET_TO_EMPTY: {
      break;
    }
    case types::eScenarioResetRequest::REQUEST_RESET_TO_DEBUG: {
      presets = {
        {types::AircraftType::F16, types::AircraftInitialConditionPreset::DEFAULT},
        {types::AircraftType::F16, types::AircraftInitialConditionPreset::DEFAULT_OPPONENT},
        {types::AircraftType::F16, types::AircraftInitialConditionPreset::ON_GROUND},
        {types::AircraftType::F16, types::AircraftInitialConditionPreset::TAKEOFF_ROLL},
        {types::AircraftType::F16, types::AircraftInitialConditionPreset::LEFT_SPIRAL},
        {types::AircraftType::F16, types::AircraftInitialConditionPreset::RIGHT_SPIRAL},
        {types::AircraftType::F16, types::AircraftInitialConditionPreset::LEFT_TAXI},
        {types::AircraftType::F16, types::AircraftInitialConditionPreset::RIGHT_TAXI},
        {types::AircraftType::F16_NO_PID, types::AircraftInitialConditionPreset::TAKEOFF_ROLL_ROTATION},
      };
      break;
    }
    case types::eScenarioResetRequest::REQUEST_RESET_TO_CLASSIC_MERGE: {
      presets = {
        {types::AircraftType::F16_NO_PID, types::AircraftInitialConditionPreset::DEFAULT},
        {types::AircraftType::F16_NO_PID, types::AircraftInitialConditionPreset::DEFAULT_OPPONENT},
      };
      break;
    }
    case types::eScenarioResetRequest::REQUEST_RESET_TO_FREE_FLIGHT: {
      presets = {
        {types::AircraftType::F16_NO_PID, types::AircraftInitialConditionPreset::DEFAULT},
      };
      break;
    }
  }

  // Wipe sim
  perform_sim_reset();

  // Apply presets and initial conditions
  types::Object3D *scene_root = blackboard->magnum_blackboard->scene_root;
  const model::GLBModelRepository *model_repo = blackboard->magnum_blackboard->model_repo;
  Magnum::SceneGraph::DrawableGroup3D *drawables = blackboard->magnum_blackboard->drawables;
  Magnum::Shaders::PhongGL *phong_shader = blackboard->magnum_blackboard->phong_shader;
  shaders::ShadowShader *shadow_shader = blackboard->magnum_blackboard->shadow_shader;

  for (auto preset : presets) {
    AircraftHandle aircraft = AircraftHandle{preset.first};
    aircraft
      .with_fdmexec()
      .with_ic(preset.second)
      .with_visual_root(new types::Object3D{scene_root})
      .with_keypoints({
        types::AircraftKeyPoints::WINGTIP_L,
        types::AircraftKeyPoints::WINGTIP_R,
        types::AircraftKeyPoints::ENGINE_EXHAUST,
        types::AircraftKeyPoints::NOSE,
      })
      .with_model(model_repo->get_aircraft_model(types::AircraftType::F16))
      .link(*phong_shader, *drawables)
      .link_trails(*scene_root, *drawables)
      .link_shadow(*shadow_shader, *drawables);
    
    blackboard->aircraft_blackboard->aircraft.push_back(std::move(aircraft));
  }

  // Debounce scenario request back to none
  blackboard->sim_state_blackboard->scenario_reset_request = types::eScenarioResetRequest::NO_REQUEST;
}
