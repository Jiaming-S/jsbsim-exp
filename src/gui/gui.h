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
      types::AircraftStateInfo state = cur_aircraft.as_aircraft_state();
      if (ImGui::BeginTable("Aircraft Info", 2, table_flags)) {
        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

        ImGui::TableNextColumn();
        ImGui::Text("Yaw");
        ImGui::TableNextColumn();
        ImGui::Text("%.2lf deg", Magnum::Float(state.yaw) / M_PI * 180.0);

        ImGui::TableNextColumn();
        ImGui::Text("Roll");
        ImGui::TableNextColumn();
        ImGui::Text("%.2lf deg", Magnum::Float(state.roll) / M_PI * 180.0);

        ImGui::TableNextColumn();
        ImGui::Text("Pitch");
        ImGui::TableNextColumn();
        ImGui::Text("%.2lf deg", Magnum::Float(state.pitch) / M_PI * 180.0);

        ImGui::TableNextColumn();
        ImGui::Text("Alt");
        ImGui::TableNextColumn();
        ImGui::Text("%.2lf ft", state.alt);

        ImGui::TableNextColumn();
        ImGui::Text("North");
        ImGui::TableNextColumn();
        ImGui::Text("%.2lf ft", state.north);

        ImGui::TableNextColumn();
        ImGui::Text("East");
        ImGui::TableNextColumn();
        ImGui::Text("%.2lf ft", state.east);

        ImGui::TableNextColumn();
        ImGui::Text("Down");
        ImGui::TableNextColumn();
        ImGui::Text("%.2lf ft", state.down);

        ImGui::TableNextColumn();
        ImGui::Text("Velocity North");
        ImGui::TableNextColumn();
        ImGui::Text("%.2lf ft/s", state.v_north);
        
        ImGui::TableNextColumn();
        ImGui::Text("Velocity East");
        ImGui::TableNextColumn();
        ImGui::Text("%.2lf ft/s", state.v_east);
        
        ImGui::TableNextColumn();
        ImGui::Text("Velocity Down");
        ImGui::TableNextColumn();
        ImGui::Text("%.2lf ft/s", state.v_down);

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
  CameraHandle &cam = blackboard->camera_blackboard->cameras[active_camera_index];
  
  const size_t active_aircraft_index = blackboard->aircraft_blackboard->active_aircraft_index;
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
      Magnum::Matrix4 prev_world_position = cam._mount->absoluteTransformation();
      Magnum::Matrix4 prev_orientation = cam._revolut->transformation();

      cam.reattach_to(scene_root);

      cam._mount->translate(prev_world_position.translation());
      cam._revolut->transform(prev_orientation);

      blackboard->sim_state_blackboard->sim_control_type = types::eSimControlType::CAMERA;
      blackboard->sim_state_blackboard->has_active_aircraft = types::eAircraftActive::NO_ACTIVE;
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
        cam.reattach_to(ac._visual_root_object);
        cam._mount->translate(cam._camera->projectionMatrix().up() * 10);
        cam._mount->translate(cam._camera->projectionMatrix().backward() * -50);

        blackboard->sim_state_blackboard->sim_control_type = types::eSimControlType::MODEL;
        blackboard->sim_state_blackboard->has_active_aircraft = types::eAircraftActive::HAS_ACTIVE;
        blackboard->aircraft_blackboard->active_aircraft_index = i;
      }
      
      // Highlight in red if active
      if (i == active_aircraft_index) {
        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(100, 0, 0, 255));
      }

      ImGui::PopID();
    }

    ImGui::EndTable();
  }

  ImGui::End();
}

// inline void gui_directional_radar_plot(
//   std::string label,
//   Magnum::Deg angle,
//   float magnitude,
//   float radar_radius = 25.0f
// ) {
//   ImGui::Text("%s", label.c_str());
  
//   ImVec2 size(radar_radius * 2, radar_radius * 2);
//   ImVec2 pos = ImGui::GetCursorScreenPos();
//   ImVec2 center = ImVec2(pos.x + radar_radius, pos.y + radar_radius);

//   ImDrawList* draw_list = ImGui::GetWindowDrawList();

//   // Radar background and scale circle
//   draw_list->AddCircleFilled(center, radar_radius, IM_COL32(30, 30, 30, 255));
//   draw_list->AddCircle(center, radar_radius, IM_COL32(150, 150, 150, 255), 32, 1.5f);
//   draw_list->AddCircle(center, radar_radius * 0.5f, IM_COL32(70, 70, 70, 255), 32, 1.0f);

//   // Crosshair
//   draw_list->AddLine(ImVec2(center.x - radar_radius, center.y), ImVec2(center.x + radar_radius, center.y), IM_COL32(70, 70, 70, 255));
//   draw_list->AddLine(ImVec2(center.x, center.y - radar_radius), ImVec2(center.x, center.y + radar_radius), IM_COL32(70, 70, 70, 255));

//   // End point
//   float line_length = magnitude / 1.0f * radar_radius;
//   ImVec2 end_point = ImVec2(
//     center.x + line_length * std::cos(Magnum::Float(Magnum::Rad(angle))),
//     center.y + line_length * std::sin(Magnum::Float(Magnum::Rad(angle)))
//   );

//   // Draw line and tip
//   draw_list->AddLine(center, end_point, IM_COL32(200, 0, 0, 255), 3.5f);
//   draw_list->AddCircleFilled(end_point, 4.0f, IM_COL32(255, 0, 0, 255));

//   // To occupy space
//   ImGui::Dummy(size);
// }

// inline void gui_input_and_control(
//   std::shared_ptr<types::SimContext> sim_context,
//   input::CommandedMovement& commanded_movement,
//   ImGuiTableFlags table_flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders
// ) {
//   ImGui::SetNextWindowSize(ImVec2(200, 400), ImGuiCond_FirstUseEver);
//   ImGui::SetNextWindowPos(ImVec2(435, 25), ImGuiCond_FirstUseEver);
//   ImGui::Begin("Input and Control");

//   ImGui::Text("Simulation Context Enums");
//   if (ImGui::BeginTable("SimContext", 2, table_flags)) {
//     ImGui::TableSetupColumn("State");
//     ImGui::TableSetupColumn("Status");

//     ImGui::TableNextColumn();
//     ImGui::Text("State");
//     ImGui::TableNextColumn();
//     ImGui::Text("%s", magic_enum::enum_name(sim_context->state).data());

//     ImGui::TableNextColumn();
//     ImGui::Text("Camera Type");
//     ImGui::TableNextColumn();
//     ImGui::Text("%s", magic_enum::enum_name(sim_context->camera_type).data());

//     ImGui::TableNextColumn();
//     ImGui::Text("Control Type");
//     ImGui::TableNextColumn();
//     ImGui::Text("%s", magic_enum::enum_name(sim_context->control_type).data());

//     ImGui::EndTable();
//   }
  
//   ImGui::Text("Commanded Controls");

//   float commanded_yaw     = -commanded_movement.yaw;
//   float commanded_pitch   =  commanded_movement.pitch;
//   float commanded_roll    = -commanded_movement.roll;
//   float commanded_forward = -commanded_movement.z;
//   float commanded_right   =  commanded_movement.x;
//   float commanded_up      =  commanded_movement.y;

//   if (ImGui::BeginTable("CommandedControls", 2, table_flags)) {
//     ImGui::TableSetupColumn("Control");
//     ImGui::TableSetupColumn("Value");

//     ImGui::TableNextColumn();
//     ImGui::Text("Yaw");
//     ImGui::TableNextColumn();
//     ImGui::Text("%f", commanded_yaw);

//     ImGui::TableNextColumn();
//     ImGui::Text("Pitch");
//     ImGui::TableNextColumn();
//     ImGui::Text("%f", commanded_pitch);

//     ImGui::TableNextColumn();
//     ImGui::Text("Roll");
//     ImGui::TableNextColumn();
//     ImGui::Text("%f", commanded_roll);

//     ImGui::TableNextColumn();
//     ImGui::Text("Forward");
//     ImGui::TableNextColumn();
//     ImGui::Text("%f", commanded_forward);

//     ImGui::TableNextColumn();
//     ImGui::Text("Right");
//     ImGui::TableNextColumn();
//     ImGui::Text("%f", commanded_right);

//     ImGui::TableNextColumn();
//     ImGui::Text("Up");
//     ImGui::TableNextColumn();
//     ImGui::Text("%f", commanded_up);

//     ImGui::EndTable();
//   }

//   float commanded_stick_movement_magnitude = std::sqrt(commanded_pitch * commanded_pitch + commanded_roll * commanded_roll);
//   Magnum::Deg commanded_stick_movement_rad = Magnum::Rad(std::atan2(commanded_pitch, commanded_roll));
//   ImGui::BeginGroup();
//   gui_directional_radar_plot(
//     "Stick",
//     commanded_stick_movement_rad,
//     commanded_stick_movement_magnitude
//   );
//   ImGui::EndGroup();

//   ImGui::SameLine(0, 20.0f);

//   ImGui::BeginGroup();
//   Magnum::Deg commanded_yaw_rad = Magnum::Rad(commanded_yaw * M_PI_2 - M_PI_2);
//   gui_directional_radar_plot(
//     "Rudder",
//     commanded_yaw_rad,
//     1.0f
//   );
//   ImGui::EndGroup();
  
//   ImGui::SameLine(0, 20.0f);
  
//   ImGui::BeginGroup();
//   ImGui::Text("Throttle");
//   ImGui::VSliderFloat(
//     "##ThrottleSlider",
//     ImVec2(20, 45),
//     &commanded_forward,
//     -1.0f,
//     1.0f
//   );
//   ImGui::EndGroup();

//   ImGui::End();
// }

// inline void gui_hud(
//   std::shared_ptr<types::SimContext> sim_context,
//   input::CommandedMovement& commanded_movement,
//   float crosshair_len = 5.0f
// ) {
//   ImGuiViewport* viewport = ImGui::GetMainViewport();
//   ImDrawList* draw_list = ImGui::GetBackgroundDrawList();

//   // Draw crosshair
//   ImVec2 center = ImVec2(
//     viewport->Pos.x + viewport->Size.x * 0.5f,
//     viewport->Pos.y + viewport->Size.y * 0.5f
//   );

//   draw_list->AddLine(
//     ImVec2(center.x - crosshair_len, center.y),
//     ImVec2(center.x + crosshair_len, center.y),
//     IM_COL32(255, 255, 255, 255),
//     1.0f
//   );
  
//   draw_list->AddLine(
//     ImVec2(center.x, center.y - crosshair_len),
//     ImVec2(center.x, center.y + crosshair_len),
//     IM_COL32(255, 255, 255, 255),
//     1.0f
//   );

//   // Draw mouse movement line
//   ImVec2 mouse_move_end_point = ImVec2(
//     center.x + commanded_movement.mouse_delta.x(),
//     center.y + commanded_movement.mouse_delta.y()
//   );

//   draw_list->AddLine(
//     center,
//     mouse_move_end_point,
//     IM_COL32(128, 0, 0, 128),
//     2.0f
//   );
//   draw_list->AddCircleFilled(
//     mouse_move_end_point,
//     4.0f,
//     IM_COL32(128, 0, 0, 128)
//   );
// }

}