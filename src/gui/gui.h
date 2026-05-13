#pragma once

#include <Magnum/ImGuiIntegration/Context.hpp>
#include <magic_enum.hpp>

#include "../aircrafthandle.h"
#include "../utils/utils.h"

using namespace Magnum::Math::Literals;


namespace gui {

void gui_aircraft_debug_collapse(
  AircraftHandle& ac,
  std::string header_name,
  ImGuiTreeNodeFlags header_flags = ImGuiTreeNodeFlags_DefaultOpen,
  ImGuiTableFlags table_flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders
) {
  if (ImGui::CollapsingHeader(header_name.c_str(), header_flags)) {
    types::AircraftStateInfo state = ac.to_aircraft_state();
    if (ImGui::BeginTable("Aircraft Info", 2, table_flags)) {
      ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);

      ImGui::TableNextColumn();
      ImGui::Text("Yaw");
      ImGui::TableNextColumn();
      ImGui::Text("%lf deg", Magnum::Float(state.yaw) / M_PI * 180.0);

      ImGui::TableNextColumn();
      ImGui::Text("Roll");
      ImGui::TableNextColumn();
      ImGui::Text("%lf deg", Magnum::Float(state.roll) / M_PI * 180.0);

      ImGui::TableNextColumn();
      ImGui::Text("Pitch");
      ImGui::TableNextColumn();
      ImGui::Text("%lf deg", Magnum::Float(state.pitch) / M_PI * 180.0);

      ImGui::TableNextColumn();
      ImGui::Text("Alt");
      ImGui::TableNextColumn();
      ImGui::Text("%lf ft", state.alt);

      ImGui::TableNextColumn();
      ImGui::Text("North");
      ImGui::TableNextColumn();
      ImGui::Text("%lf ft", state.north);

      ImGui::TableNextColumn();
      ImGui::Text("East");
      ImGui::TableNextColumn();
      ImGui::Text("%lf ft", state.east);

      ImGui::TableNextColumn();
      ImGui::Text("Down");
      ImGui::TableNextColumn();
      ImGui::Text("%lf ft", state.down);

      ImGui::TableNextColumn();
      ImGui::Text("Velocity North");
      ImGui::TableNextColumn();
      ImGui::Text("%lf ft/s", state.v_north);
      
      ImGui::TableNextColumn();
      ImGui::Text("Velocity East");
      ImGui::TableNextColumn();
      ImGui::Text("%lf ft/s", state.v_east);
      
      ImGui::TableNextColumn();
      ImGui::Text("Velocity Down");
      ImGui::TableNextColumn();
      ImGui::Text("%lf ft/s", state.v_down);

      ImGui::EndTable();
    }
  }
}

void gui_aircraft_debug(std::vector<AircraftHandle>& aircraft) {
  ImGui::SetNextWindowSize(ImVec2(200, 1000), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowPos(ImVec2(25, 25), ImGuiCond_FirstUseEver);
  ImGui::Begin("Debug");

  for (size_t i = 0; i < aircraft.size(); i++) {
    auto& ac = aircraft[i];
    gui_aircraft_debug_collapse(ac, std::to_string(i) + ": " + ac._aircraft_type_string);
  }

  ImGui::End();
}


void gui_camera_selection(
  std::vector<AircraftHandle>& aircraft,
  CameraHandle& cam,
  types::Scene3D& scene
) {
  ImGui::SetNextWindowSize(ImVec2(200, 400), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowPos(ImVec2(230, 25), ImGuiCond_FirstUseEver);
  ImGui::Begin("Camera Selection");

  if (ImGui::BeginTable("Aircraft", 2)) {
    ImGui::TableSetupColumn("Choice");
    ImGui::TableSetupColumn("Button");

    ImGui::TableNextColumn();
    ImGui::Text("Reset Camera");
    ImGui::TableNextColumn();
    
    ImGui::PushID(-1);
    if (ImGui::Button("Reset")) {
      cam.reattach_to(&scene);
    }
    ImGui::PopID();

    for (size_t i = 0; i < aircraft.size(); i++) {
      auto& ac = aircraft[i];
      std::string aircraft_string_id = std::to_string(i) + ": " + ac._aircraft_type_string;

      ImGui::TableNextColumn();
      ImGui::Text("%s", aircraft_string_id.c_str());
      ImGui::TableNextColumn();
      
      ImGui::PushID(i);
      if (ImGui::Button("Bind Camera")) cam.reattach_to(ac._visual_root_object);
      ImGui::PopID();
    }

    ImGui::EndTable();
  }

  ImGui::End();
}

void gui_directional_radar_plot(
  std::string label,
  Magnum::Deg angle,
  float magnitude,
  float radar_radius = 25.0f
) {
  ImGui::Text("%s", label.c_str());
  
  ImVec2 size(radar_radius * 2, radar_radius * 2);
  ImVec2 pos = ImGui::GetCursorScreenPos();
  ImVec2 center = ImVec2(pos.x + radar_radius, pos.y + radar_radius);

  ImDrawList* draw_list = ImGui::GetWindowDrawList();

  // Radar background and scale circle
  draw_list->AddCircleFilled(center, radar_radius, IM_COL32(30, 30, 30, 255));
  draw_list->AddCircle(center, radar_radius, IM_COL32(150, 150, 150, 255), 32, 1.5f);
  draw_list->AddCircle(center, radar_radius * 0.5f, IM_COL32(70, 70, 70, 255), 32, 1.0f);

  // Crosshair
  draw_list->AddLine(ImVec2(center.x - radar_radius, center.y), ImVec2(center.x + radar_radius, center.y), IM_COL32(70, 70, 70, 255));
  draw_list->AddLine(ImVec2(center.x, center.y - radar_radius), ImVec2(center.x, center.y + radar_radius), IM_COL32(70, 70, 70, 255));

  // End point
  float line_length = magnitude / 1.0f * radar_radius;
  ImVec2 end_point = ImVec2(
    center.x + line_length * std::cos(Magnum::Float(Magnum::Rad(angle))),
    center.y + line_length * std::sin(Magnum::Float(Magnum::Rad(angle)))
  );

  // Draw line and tip
  draw_list->AddLine(center, end_point, IM_COL32(200, 0, 0, 255), 3.5f);
  draw_list->AddCircleFilled(end_point, 4.0f, IM_COL32(255, 0, 0, 255));

  // To occupy space
  ImGui::Dummy(size);
}

void gui_input_and_control(
  std::shared_ptr<types::SimContext> sim_context,
  input::CommandedMovement& commanded_movement,
  ImGuiTableFlags table_flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders
) {
  ImGui::SetNextWindowSize(ImVec2(200, 400), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowPos(ImVec2(435, 25), ImGuiCond_FirstUseEver);
  ImGui::Begin("Input and Control");

  ImGui::Text("Simulation Context Enums");
  if (ImGui::BeginTable("SimContext", 2, table_flags)) {
    ImGui::TableSetupColumn("State");
    ImGui::TableSetupColumn("Status");

    ImGui::TableNextColumn();
    ImGui::Text("State");
    ImGui::TableNextColumn();
    ImGui::Text("%s", magic_enum::enum_name(sim_context->state).data());

    ImGui::TableNextColumn();
    ImGui::Text("Camera Type");
    ImGui::TableNextColumn();
    ImGui::Text("%s", magic_enum::enum_name(sim_context->camera_type).data());

    ImGui::TableNextColumn();
    ImGui::Text("Control Type");
    ImGui::TableNextColumn();
    ImGui::Text("%s", magic_enum::enum_name(sim_context->control_type).data());

    ImGui::EndTable();
  }
  
  ImGui::Text("Commanded Controls");

  float commanded_yaw     = -commanded_movement.yaw;
  float commanded_pitch   =  commanded_movement.pitch;
  float commanded_roll    = -commanded_movement.roll;
  float commanded_forward = -commanded_movement.z;
  float commanded_right   =  commanded_movement.x;
  float commanded_up      =  commanded_movement.y;

  if (ImGui::BeginTable("CommandedControls", 2, table_flags)) {
    ImGui::TableSetupColumn("Control");
    ImGui::TableSetupColumn("Value");

    ImGui::TableNextColumn();
    ImGui::Text("Yaw");
    ImGui::TableNextColumn();
    ImGui::Text("%f", commanded_yaw);

    ImGui::TableNextColumn();
    ImGui::Text("Pitch");
    ImGui::TableNextColumn();
    ImGui::Text("%f", commanded_pitch);

    ImGui::TableNextColumn();
    ImGui::Text("Roll");
    ImGui::TableNextColumn();
    ImGui::Text("%f", commanded_roll);

    ImGui::TableNextColumn();
    ImGui::Text("Forward");
    ImGui::TableNextColumn();
    ImGui::Text("%f", commanded_forward);

    ImGui::TableNextColumn();
    ImGui::Text("Right");
    ImGui::TableNextColumn();
    ImGui::Text("%f", commanded_right);

    ImGui::TableNextColumn();
    ImGui::Text("Up");
    ImGui::TableNextColumn();
    ImGui::Text("%f", commanded_up);

    ImGui::EndTable();
  }

  float commanded_stick_movement_magnitude = std::sqrt(commanded_pitch * commanded_pitch + commanded_roll * commanded_roll);
  Magnum::Deg commanded_stick_movement_rad = Magnum::Rad(std::atan2(commanded_pitch, commanded_roll));
  ImGui::BeginGroup();
  gui_directional_radar_plot(
    "Stick",
    commanded_stick_movement_rad,
    commanded_stick_movement_magnitude
  );
  ImGui::EndGroup();

  ImGui::SameLine(0, 20.0f);

  ImGui::BeginGroup();
  Magnum::Deg commanded_yaw_rad = Magnum::Rad(commanded_yaw / 4.0f * M_PI_2 - M_PI_2);
  gui_directional_radar_plot(
    "Rudder",
    commanded_yaw_rad,
    1.0f
  );
  ImGui::EndGroup();
  
  ImGui::SameLine(0, 20.0f);
  
  ImGui::BeginGroup();
  ImGui::Text("Throttle");
  ImGui::VSliderFloat(
    "##ThrottleSlider",
    ImVec2(20, 45),
    &commanded_forward,
    0.0f,
    1.0f
  );
  ImGui::EndGroup();

  ImGui::End();
}

}