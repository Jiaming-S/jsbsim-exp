#include "gui_component.h"

void GuiComponent::init() {}

void GuiComponent::quit() {}

void GuiComponent::handle_dispatch() {
  Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::Blending);
  Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::ScissorTest);
  Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::FaceCulling);
  Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::DepthTest);
  Magnum::GL::Renderer::setBlendEquation(Magnum::GL::Renderer::BlendEquation::Add, Magnum::GL::Renderer::BlendEquation::Add);
  Magnum::GL::Renderer::setBlendFunction(Magnum::GL::Renderer::BlendFunction::SourceAlpha, Magnum::GL::Renderer::BlendFunction::OneMinusSourceAlpha);

  const types::eCursorHidden cursor_hidden = blackboard->sim_state_blackboard->cursor_hidden;
  Magnum::ImGuiIntegration::Context *imgui_ctx = blackboard->magnum_blackboard->imgui_ctx;

  gui::gui_aircraft_debug(blackboard);
  gui::gui_camera_selection(blackboard);
  // gui::gui_input_and_control(_sim_context, commanded_movement);
  // gui::gui_hud(_sim_context, commanded_movement);

  if (cursor_hidden == types::eCursorHidden::VISIBLE) {
    imgui_ctx->updateApplicationCursor(*app);
  }
  imgui_ctx->drawFrame();

  Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::ScissorTest);
  Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::Blending);
  Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::DepthTest);
  Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::FaceCulling);

}