#include "gui_component.h"

void GuiComponent::init() {}

void GuiComponent::quit() {}

void GuiComponent::handle_dispatch() {
  gui::gui_aircraft_debug(blackboard);
  gui::gui_camera_selection(blackboard);
  // gui::gui_input_and_control(_sim_context, commanded_movement);
  // gui::gui_hud(_sim_context, commanded_movement);


}