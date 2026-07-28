#include "mouse_cursor_hide_component.h"

void MouseCursorHideComponent::init() {}

void MouseCursorHideComponent::quit() {}

void MouseCursorHideComponent::handle_dispatch() {
  const types::eCursorHidden cursor_hidden = blackboard->sim_state_blackboard->cursor_hidden;
  
  // Lock cursor to center of screen
  if (cursor_hidden == types::eCursorHidden::HIDDEN_AND_LOCKED) {
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    
    // Scuffed but works
    // TODO: why is setting `HiddenLocked` not enough by itself????
    app->warpCursor(app->windowSize() / 2);
    app->setCursor(Sdl2Application::Cursor::HiddenLocked);
  }

  // Normal cursor
  else if (cursor_hidden == types::eCursorHidden::VISIBLE) {
    ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;
    app->setCursor(Sdl2Application::Cursor::Arrow);
  }
}
