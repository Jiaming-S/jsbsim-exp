#pragma once

#include <Magnum/ImGuiIntegration/Context.hpp>

#include "../aircrafthandle.h"


namespace gui {

void gui_aircraft_debug_collapse(
  AircraftHandle& ac,
  std::string header_name,
  ImGuiTreeNodeFlags header_flags = ImGuiTreeNodeFlags_DefaultOpen,
  ImGuiTableFlags table_flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders
) {
  if (ImGui::CollapsingHeader(header_name.c_str(), header_flags)) {
    _AircraftStateInfo state = ac.to_aircraft_state();
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

    ImGui::Button("Bind Camera");
  }
}

void gui_aircraft(std::vector<AircraftHandle>& aircraft) {
  ImGui::SetNextWindowSize(ImVec2(200, 1000), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_FirstUseEver);
  ImGui::Begin("Debug");

  for (size_t i = 0; i < aircraft.size(); i++) {
    auto& ac = aircraft[i];
    gui_aircraft_debug_collapse(ac, std::to_string(i) + ": " + ac._aircraft_type_string);
  }

  ImGui::End();
}

}