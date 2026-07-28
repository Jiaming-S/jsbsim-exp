#include "mouse_cursor_hide_component.h"

void MouseCursorHideComponent::init() {}

void MouseCursorHideComponent::quit() {}

void MouseCursorHideComponent::handle_dispatch() {
  const types::eCursorHidden cursor_hidden = blackboard->sim_state_blackboard->cursor_hidden;

  if (cursor_hidden == types::eCursorHidden::HIDDEN_AND_LOCKED) {
    ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;
    app->setCursor(Sdl2Application::Cursor::HiddenLocked);
  }

  else if (cursor_hidden == types::eCursorHidden::VISIBLE) {
    ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;
    app->setCursor(Sdl2Application::Cursor::Arrow);
  }


}
