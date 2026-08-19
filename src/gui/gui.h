#pragma once

#include "Magnum/Magnum.h"
#include <Magnum/ImGuiIntegration/Context.hpp>
#include <magic_enum.hpp>
#include <memory>
#include <vector>

#include "../blackboard/jsbsimexpblackboard.h"
#include "../aircrafthandle.h"
#include "../camerahandle.h"

using namespace Magnum::Math::Literals;


namespace gui {

inline void gui_aircraft_debug(
  std::shared_ptr<JSBSimExpBlackboard> &blackboard,
  ImGuiTableFlags table_flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders
) {
  ImGui::SetNextWindowSize(ImVec2(200, 1000), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowPos(ImVec2(25, 25), ImGuiCond_FirstUseEver);
  ImGui::Begin("Debug");

  for (size_t i = 0; i < blackboard->aircraft_blackboard->aircraft.size(); i++) {
    const AircraftHandle& cur_aircraft = blackboard->aircraft_blackboard->aircraft[i];
    const std::string cur_aircraft_string = std::to_string(i) + ": " + cur_aircraft._aircraft_type_string;
    const bool cur_is_active = (i == blackboard->aircraft_blackboard->active_aircraft_index);
    
    // Open dropdown automatically when selecting a new active aircraft
    if (cur_is_active) ImGui::SetNextItemOpen(true, ImGuiCond_Once);

    if (ImGui::CollapsingHeader(cur_aircraft_string.c_str())) {
      const types::AircraftStateInfo state = cur_aircraft._state_info;
      if (ImGui::BeginTable("Aircraft Info", 2, table_flags)) {
        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

        ImGui::TableNextColumn();
        ImGui::Text("Yaw");
        ImGui::TableNextColumn();
        ImGui::Text("%.2f deg", (float) state.yaw_deg);

        ImGui::TableNextColumn();
        ImGui::Text("Pitch");
        ImGui::TableNextColumn();
        ImGui::Text("%.2f deg", (float) state.pitch_deg);

        ImGui::TableNextColumn();
        ImGui::Text("Roll");
        ImGui::TableNextColumn();
        ImGui::Text("%.2f deg", (float) state.roll_deg);

        ImGui::TableNextColumn();
        ImGui::Text("AOA");
        ImGui::TableNextColumn();
        ImGui::Text("%.2f deg", (float) state.alpha_deg);

        ImGui::TableNextColumn();
        ImGui::Text("Alt");
        ImGui::TableNextColumn();
        ImGui::Text("%.2lf km", state.alt_km);

        ImGui::TableNextColumn();
        ImGui::Text("North");
        ImGui::TableNextColumn();
        ImGui::Text("%.2lf km", state.north_km);

        ImGui::TableNextColumn();
        ImGui::Text("East");
        ImGui::TableNextColumn();
        ImGui::Text("%.2lf km", state.east_km);

        ImGui::TableNextColumn();
        ImGui::Text("Down");
        ImGui::TableNextColumn();
        ImGui::Text("%.2lf km", state.down_km);

        ImGui::TableNextColumn();
        ImGui::Text("Vel North");
        ImGui::TableNextColumn();
        ImGui::Text("%.2lf kph", state.north_spd_kph);
        
        ImGui::TableNextColumn();
        ImGui::Text("Vel East");
        ImGui::TableNextColumn();
        ImGui::Text("%.2lf kph", state.east_spd_kph);
        
        ImGui::TableNextColumn();
        ImGui::Text("Vel Down");
        ImGui::TableNextColumn();
        ImGui::Text("%.2lf kph", state.down_spd_kph);

        ImGui::TableNextColumn();
        ImGui::Text("Ground Spd");
        ImGui::TableNextColumn();
        ImGui::Text("%.2lf kph", state.ground_spd_kph);

        ImGui::TableNextColumn();
        ImGui::Text("Indicated Spd");
        ImGui::TableNextColumn();
        ImGui::Text("%.2lf kph", state.indicated_spd_kph);

        ImGui::EndTable();
      }
    }
  }

  ImGui::End();
}


inline void gui_camera_selection(
  std::shared_ptr<JSBSimExpBlackboard> &blackboard
) {
  ImGui::SetNextWindowSize(ImVec2(200, 400), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowPos(ImVec2(230, 25), ImGuiCond_FirstUseEver);
  ImGui::Begin("Camera Selection");

  const size_t active_camera_index = blackboard->camera_blackboard->active_camera_index;
  CameraHandle &camera_handle = blackboard->camera_blackboard->cameras[active_camera_index];
  
  const size_t active_aircraft_index = blackboard->aircraft_blackboard->active_aircraft_index;
  const bool has_active_aircraft = blackboard->sim_state_blackboard->has_active_aircraft == types::eAircraftActive::HAS_ACTIVE;
  const std::vector<AircraftHandle> &aircraft = blackboard->aircraft_blackboard->aircraft;

  types::Object3D *scene_root = blackboard->magnum_blackboard->scene_root;

  if (ImGui::BeginTable("Aircraft", 2)) {
    ImGui::TableSetupColumn("Choice");
    ImGui::TableSetupColumn("Button");

    ImGui::TableNextColumn();
    ImGui::Text("Detach Camera");
    ImGui::TableNextColumn();
    
    
    // Detach from aircraft, and stay at current world position
    ImGui::PushID(-1);
    if (ImGui::Button("Detach")) {
      // Detach _mount from aircraft root and back to scene root
      camera_handle.attach_to_scene_root(scene_root);

      // Update relevant sim states
      blackboard->sim_state_blackboard->has_active_aircraft = types::eAircraftActive::NO_ACTIVE;
      blackboard->sim_state_blackboard->sim_control_type_default = types::eSimControlType::CAMERA;
    }
    ImGui::PopID();

    // Add this selection for all aircraft
    for (size_t i = 0; i < aircraft.size(); i++) {
      auto& ac = aircraft[i];
      const std::string cur_aircraft_string = std::to_string(i) + ": " + ac._aircraft_type_string;

      ImGui::TableNextColumn();
      ImGui::Text("%s", cur_aircraft_string.c_str());
      ImGui::TableNextColumn();
      
      ImGui::PushID(i);
      if (ImGui::Button("Bind Camera")) {
        // Attach _mount to aircraft root
        camera_handle
          .attach_to(ac._visual_root_object)
          .with_default_offset(types::eSimControlType::MODEL);

        // Update relevant sim states
        blackboard->sim_state_blackboard->sim_control_type_default = types::eSimControlType::MODEL;
        blackboard->sim_state_blackboard->has_active_aircraft = types::eAircraftActive::HAS_ACTIVE;
        blackboard->aircraft_blackboard->active_aircraft_index = i;
      }
      
      // Highlight in red if active
      if (i == active_aircraft_index && has_active_aircraft) {
        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(100, 0, 0, 255));
      }

      ImGui::PopID();
    }

    ImGui::EndTable();
  }

  ImGui::End();
}


inline void gui_sim_state(
  std::shared_ptr<JSBSimExpBlackboard> &blackboard,
  ImGuiTableFlags table_flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders
) {
  ImGui::SetNextWindowSize(ImVec2(320, 400), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowPos(ImVec2(440, 25), ImGuiCond_FirstUseEver);
  ImGui::Begin("Sim State");

  if (ImGui::BeginTable("Sim State Info", 2, table_flags)) {
    ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

    ImGui::TableNextColumn();
    ImGui::Text("Physics State");
    ImGui::TableNextColumn();
    ImGui::Text("%s", magic_enum::enum_name(blackboard->sim_state_blackboard->sim_physics_state).data());

    ImGui::TableNextColumn();
    ImGui::Text("Control Type");
    ImGui::TableNextColumn();
    ImGui::Text("%s", magic_enum::enum_name(blackboard->sim_state_blackboard->sim_control_type).data());

    ImGui::TableNextColumn();
    ImGui::Text("Control Type Default");
    ImGui::TableNextColumn();
    ImGui::Text("%s", magic_enum::enum_name(blackboard->sim_state_blackboard->sim_control_type_default).data());

    ImGui::TableNextColumn();
    ImGui::Text("Camera Type");
    ImGui::TableNextColumn();
    ImGui::Text("%s", magic_enum::enum_name(blackboard->sim_state_blackboard->camera_type).data());

    ImGui::TableNextColumn();
    ImGui::Text("Has Active Aircraft");
    ImGui::TableNextColumn();
    ImGui::Text("%s", magic_enum::enum_name(blackboard->sim_state_blackboard->has_active_aircraft).data());

    ImGui::TableNextColumn();
    ImGui::Text("Cursor Hidden");
    ImGui::TableNextColumn();
    ImGui::Text("%s", magic_enum::enum_name(blackboard->sim_state_blackboard->cursor_hidden).data());

    ImGui::TableNextColumn();
    ImGui::Text("Debug Views");
    ImGui::TableNextColumn();
    ImGui::Text("%s", magic_enum::enum_name(blackboard->sim_state_blackboard->debug_views).data());

    ImGui::TableNextColumn();
    ImGui::Text("Telemetry Mode");
    ImGui::TableNextColumn();
    ImGui::Text("%s", magic_enum::enum_name(blackboard->sim_state_blackboard->telemetry_mode).data());

    ImGui::TableNextColumn();
    ImGui::Text("Telemetry Delivery Status");
    ImGui::TableNextColumn();
    ImGui::Text("%s", magic_enum::enum_name(blackboard->sim_state_blackboard->telemetry_delivery_status).data());

    ImGui::TableNextColumn();
    ImGui::Text("Hand Of God Mode");
    ImGui::TableNextColumn();
    ImGui::Text("%s", magic_enum::enum_name(blackboard->sim_state_blackboard->hand_of_god_mode).data());

    ImGui::TableNextColumn();
    ImGui::Text("Hand Of God Receipt Status");
    ImGui::TableNextColumn();
    ImGui::Text("%s", magic_enum::enum_name(blackboard->sim_state_blackboard->hand_of_god_receipt_status).data());

    ImGui::TableNextColumn();
    ImGui::Text("Scenario Reset Request");
    ImGui::TableNextColumn();
    ImGui::Text("%s", magic_enum::enum_name(blackboard->sim_state_blackboard->scenario_reset_request).data());

    ImGui::EndTable();
  }

  ImGui::End();
}


}