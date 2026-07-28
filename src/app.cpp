#include "app.h"
#include "types/types.h"

JSBSimVisualizer::JSBSimVisualizer(const Arguments& arguments)
  : Magnum::Platform::Application{
      arguments,
      Configuration{}
        .setTitle("Visualizer")
        .addWindowFlags(Magnum::Platform::Sdl2Application::Configuration::WindowFlag::FullscreenDesktop)
    },
    // Initialize smart pointer to blackboard
    _blackboard{make_jsbsimexp_blackboard()},
    // Initialize all components hooked onto blackboard
    _aircraft_movement_component{_blackboard, this},
    _gui_component{_blackboard, this},
    _camera_movement_component{_blackboard, this},
    _keyboard_input_component{_blackboard, this},
    _mouse_cursor_hide_component{_blackboard, this},
    _mouse_input_component{_blackboard, this},
    _sim_tick_component{_blackboard, this},
    _vis_tick_component{_blackboard, this}
{
  // Enable depth test
  Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::DepthTest);

  // Initialize shaders
  _flat_shader = Magnum::Shaders::FlatGL3D{};
  _phong_shader = Magnum::Shaders::PhongGL{
    Magnum::Shaders::PhongGL::Configuration{}
      .setFlags(Magnum::Shaders::PhongGL::Flag::DiffuseTexture)
  };


  // Initialize ImGui
  _imgui = Magnum::ImGuiIntegration::Context(
    Magnum::Vector2{windowSize()} / dpiScaling(),
    windowSize(),
    framebufferSize()
  );

  // Disable ImGui imgui.ini file
  ImGui::GetIO().IniFilename = nullptr;
  // Disable ImGui fighting Magnum for cursor control
  ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

  // Load and ingest GLTF models
  std::vector<std::pair<std::string, std::string>> models_to_import = {
    {"f16", "assets/f16/model/f16.glb"},
  };

  for (auto& p : models_to_import) {
    std::string asset_name = p.first;
    std::string asset_filepath = p.second;
    _model_repo.ingest_asset_glb(asset_name, asset_filepath);
  }

  // Load and setup camera
  CameraHandle cam = CameraHandle{
    &_scene,
    Magnum::Matrix4::perspectiveProjection(
      120.0_degf,
      Magnum::Vector2{windowSize()}.aspectRatio(),
      0.01f,
      INFINITY
    )
  };
  
  _blackboard->camera_blackboard->cameras.push_back(cam);
  cam._mount->translate(cam._camera->projectionMatrix().up() * 10);
  cam._mount->translate(cam._camera->projectionMatrix().backward() * 10);

  // Load and populate aircraft configs
  utils::populate_tmp_jsbsim_dir(types::AircraftType::F16);
  utils::populate_tmp_jsbsim_dir(types::AircraftType::F16_NO_PID);

  // Load initial conditions
  std::vector<types::AircraftInitialConditionPreset> presets = {
    types::AircraftInitialConditionPreset::DEFAULT,
    types::AircraftInitialConditionPreset::DEFAULT_OPPONENT,
    types::AircraftInitialConditionPreset::ON_GROUND,
    types::AircraftInitialConditionPreset::TAKEOFF_ROLL,
    types::AircraftInitialConditionPreset::LEFT_SPIRAL,
    types::AircraftInitialConditionPreset::RIGHT_SPIRAL,
    types::AircraftInitialConditionPreset::LEFT_TAXI,
    types::AircraftInitialConditionPreset::RIGHT_TAXI,
  };

  for (auto preset : presets) {
    AircraftHandle aircraft = AircraftHandle{types::AircraftType::F16};
    aircraft
      .with_fdmexec()
      .with_ic(preset)
      .with_visual_root(new types::Object3D{&_scene})
      .with_keypoints({
        types::AircraftKeyPoints::WINGTIP_L,
        types::AircraftKeyPoints::WINGTIP_R,
        types::AircraftKeyPoints::ENGINE_EXHAUST,
        types::AircraftKeyPoints::NOSE,
      })
      .with_model(_model_repo.get_aircraft_model(types::AircraftType::F16))
      .link(_phong_shader, _drawables)
      .link_trails(_scene, _drawables)
      .link_shadow(_shadow_shader, _drawables);
    _blackboard->aircraft_blackboard->aircraft.push_back(std::move(aircraft));
  }

  { // Add an F16 with all PID's turned off
    AircraftHandle funny_aircraft = AircraftHandle{types::AircraftType::F16_NO_PID};
    funny_aircraft
      .with_fdmexec()
      .with_ic(types::AircraftInitialConditionPreset::TAKEOFF_ROLL_ROTATION)
      .with_visual_root(new types::Object3D{&_scene})
      .with_keypoints({
        types::AircraftKeyPoints::WINGTIP_L,
        types::AircraftKeyPoints::WINGTIP_R,
        types::AircraftKeyPoints::ENGINE_EXHAUST,
        types::AircraftKeyPoints::NOSE,
      })
      .with_model(_model_repo.get_aircraft_model(types::AircraftType::F16))
      .link(_phong_shader, _drawables)
      .link_trails(_scene, _drawables)
      .link_shadow(_shadow_shader, _drawables);
    _blackboard->aircraft_blackboard->aircraft.push_back(std::move(funny_aircraft));
  }

  // Initialize environment
  _blackboard->magnum_blackboard->scene_root = new types::Object3D(&_scene);
  _atmosphere = new drawn::AtmosphereDrawable(
    *_blackboard->magnum_blackboard->scene_root,
    _sky_shader,
    _background_drawables
  );
  _environment = new drawn::EnvironmentDrawable(
    *_blackboard->magnum_blackboard->scene_root,
    _floor_shader,
    _background_drawables
  );

  // Start Magnum timeline
  _timeline.start();
  
  // Enable VSync
  setSwapInterval(1);
}

// Tick sim and visual model
void JSBSimVisualizer::tickEvent() {
  types::eSimPhysicsState sim_physics_state = _blackboard->sim_state_blackboard->sim_physics_state;
  if (sim_physics_state != types::eSimPhysicsState::PAUSED) {
    _sim_tick_component.handle_dispatch();
  }

  _vis_tick_component.handle_dispatch();
}

// Tick frame
void JSBSimVisualizer::drawEvent() {
  // Clear framebuffer
  Magnum::GL::defaultFramebuffer.clear(Magnum::GL::FramebufferClear::Color | Magnum::GL::FramebufferClear::Depth);
  
  // Next frame
  _timeline.nextFrame();
  _imgui.newFrame();

  // ImGui settings
  if ( ImGui::GetIO().WantTextInput && !isTextInputActive()) startTextInput();
  if (!ImGui::GetIO().WantTextInput &&  isTextInputActive()) stopTextInput();

  // Capture and apply user keyboard and mouse input
  _keyboard_input_component.handle_dispatch();
  _mouse_input_component.handle_dispatch();

  // Hide or unhide cursor
  _mouse_cursor_hide_component.handle_dispatch();

  // Apply keyboard and mouse input on aircraft and camera
  _aircraft_movement_component.handle_dispatch();
  _camera_movement_component.handle_dispatch();

  // Do camera draw
  // TODO: make a method for this
  _blackboard->camera_blackboard->cameras[0]._camera->draw(_background_drawables);
  _blackboard->camera_blackboard->cameras[0]._camera->draw(_drawables);

  {
    Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::Blending);
    Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::ScissorTest);
    Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::FaceCulling);
    Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::DepthTest);
    Magnum::GL::Renderer::setBlendEquation(Magnum::GL::Renderer::BlendEquation::Add, Magnum::GL::Renderer::BlendEquation::Add);
    Magnum::GL::Renderer::setBlendFunction(Magnum::GL::Renderer::BlendFunction::SourceAlpha, Magnum::GL::Renderer::BlendFunction::OneMinusSourceAlpha);

    // Draw imgui guis
    _gui_component.handle_dispatch();
    if (_blackboard->sim_state_blackboard->cursor_hidden == types::eCursorHidden::VISIBLE) {
      _imgui.updateApplicationCursor(*this);
    }
    _imgui.drawFrame();

    Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::ScissorTest);
    Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::Blending);
    Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::DepthTest);
    Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::FaceCulling);
  }

  // Next
  swapBuffers();
  redraw();
}

void JSBSimVisualizer::keyPressEvent(KeyEvent& event) {
  if (_imgui.handleKeyPressEvent(event)) return;
  _blackboard->keyboard_input_blackboard->keys_down.insert(event.key());
  event.setAccepted();
}

void JSBSimVisualizer::keyReleaseEvent(KeyEvent& event) {
  if (_imgui.handleKeyReleaseEvent(event)) return;
  _blackboard->keyboard_input_blackboard->keys_down.erase(event.key());
  event.setAccepted();
}

void JSBSimVisualizer::pointerPressEvent(PointerEvent& event) {
  if (_imgui.handlePointerPressEvent(event)) return;
  
  if (event.pointer() == Sdl2Application::Pointer::MouseLeft) {
    _blackboard->keyboard_input_blackboard->mouse_held = true;
    event.setAccepted();
  }
}

void JSBSimVisualizer::pointerReleaseEvent(PointerEvent& event) {
  if (_imgui.handlePointerReleaseEvent(event)) return;

  if (event.pointer() == Sdl2Application::Pointer::MouseLeft) {
    _blackboard->keyboard_input_blackboard->mouse_held = false;
    event.setAccepted();
  }
}

void JSBSimVisualizer::pointerMoveEvent(PointerMoveEvent& event) {
  if (_blackboard->sim_state_blackboard->cursor_hidden == types::eCursorHidden::VISIBLE) {
    if (_imgui.handlePointerMoveEvent(event)) return;
  }
  _blackboard->keyboard_input_blackboard->mouse_position = event.position();
  _blackboard->keyboard_input_blackboard->mouse_delta = event.relativePosition();
  event.setAccepted();
}

void JSBSimVisualizer::scrollEvent(ScrollEvent& event) {
  // Don't scroll if we are scrolling an ImGui window
  if (_imgui.handleScrollEvent(event)) {
    event.setAccepted();
    return;
  }
}

void JSBSimVisualizer::viewportEvent(ViewportEvent& event) {
  Magnum::GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});
  _imgui.relayout(
    Magnum::Vector2{
      event.windowSize()} / event.dpiScaling(),
      event.windowSize(), 
      event.framebufferSize()
    );
}

void JSBSimVisualizer::textInputEvent(TextInputEvent& event) {
  if (_imgui.handleTextInputEvent(event)) return;
}

