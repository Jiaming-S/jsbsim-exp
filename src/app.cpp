#include "app.h"

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
    _aircraft_control_component{_blackboard, this},
    _aircraft_state_info_component{_blackboard, this},
    _camera_control_component{_blackboard, this},
    _camera_draw_component{_blackboard, this},
    _foreign_input_component{_blackboard, this},
    _gui_component{_blackboard, this},
    _hand_of_god_tick_component{_blackboard, this},
    _keyboard_input_component{_blackboard, this},
    _mouse_cursor_hide_component{_blackboard, this},
    _mouse_input_component{_blackboard, this},
    _sim_tick_component{_blackboard, this},
    _telemetry_tick_component{_blackboard, this},
    _vis_tick_component{_blackboard, this}
{
  // Enable depth test
  Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::DepthTest);

  // Initialize shaders
  _phong_shader = Magnum::Shaders::PhongGL{
    Magnum::Shaders::PhongGL::Configuration{}
      .setFlags(Magnum::Shaders::PhongGL::Flag::DiffuseTexture)
  };

  // Initialize ImGui
  _imgui_ctx = Magnum::ImGuiIntegration::Context(
    Magnum::Vector2{windowSize()} / dpiScaling(),
    windowSize(),
    framebufferSize()
  );

  // Disable ImGui imgui.ini file
  ImGui::GetIO().IniFilename = nullptr;
  // Disable ImGui fighting Magnum for cursor control
  ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

  { // Load and ingest GLTF models
    _model_repo.ingest_asset_glb("f16", "assets/f16/3d/f16.glb");
  }

  { // Load and setup camera
    CameraHandle camera_handle = CameraHandle{&_scene};
    Magnum::Matrix4 default_projection = Magnum::Matrix4::perspectiveProjection(
      _blackboard->camera_blackboard->default_camera_fov,
      Magnum::Vector2{windowSize()}.aspectRatio(),
      _blackboard->camera_blackboard->default_camera_near_dist,
      _blackboard->camera_blackboard->default_camera_far_dist
    );

    camera_handle
      .with_projection_matrix(default_projection)
      .with_default_offset(types::eSimControlType::CAMERA);
    
    _blackboard->camera_blackboard->cameras.push_back(camera_handle);
  }

  // Load and populate aircraft configs
  utils::populate_tmp_jsbsim_dir(types::AircraftType::F16);
  utils::populate_tmp_jsbsim_dir(types::AircraftType::F16_NO_PID);

  { // Load initial conditions
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

  { // Open and bind telemetry port
    _telemetry_pub_socket = zmq::socket_t{_zmq_ctx, zmq::socket_type::pub};
    _telemetry_pub_socket.bind("tcp://127.0.0.1:5555");
  }

  { // Initialize blackboard
    _blackboard->magnum_blackboard->scene_root = new types::Object3D(&_scene);
    _blackboard->magnum_blackboard->imgui_ctx = &_imgui_ctx;
    _blackboard->magnum_blackboard->_drawables = &_drawables;
    _blackboard->magnum_blackboard->_background_drawables = &_background_drawables;
    _blackboard->network_blackboard->zmq_ctx = &_zmq_ctx;
    _blackboard->network_blackboard->telemetry_pub_socket = &_telemetry_pub_socket;
  }

  { // Initialize environment
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
  }

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

  _aircraft_state_info_component.handle_dispatch();
  _vis_tick_component.handle_dispatch();

  // Send telemetry
  _telemetry_tick_component.handle_dispatch();
}

// Tick frame
void JSBSimVisualizer::drawEvent() {
  // Clear framebuffer
  Magnum::GL::defaultFramebuffer.clear(Magnum::GL::FramebufferClear::Color | Magnum::GL::FramebufferClear::Depth);
  
  // Next frame
  _timeline.nextFrame();
  _imgui_ctx.newFrame();

  // ImGui settings
  if ( ImGui::GetIO().WantTextInput && !isTextInputActive()) startTextInput();
  if (!ImGui::GetIO().WantTextInput &&  isTextInputActive()) stopTextInput();

  // Capture and apply user keyboard and mouse input
  _keyboard_input_component.handle_dispatch();
  _mouse_input_component.handle_dispatch();

  // Hide or unhide cursor
  _mouse_cursor_hide_component.handle_dispatch();

  // Check if an external process has provided input
  _foreign_input_component.handle_dispatch();

  // Apply input on aircraft and camera
  _aircraft_control_component.handle_dispatch();
  _camera_control_component.handle_dispatch();

  // Draw camera
  _camera_draw_component.handle_dispatch();

  // Draw imgui guis
  _gui_component.handle_dispatch();

  // Next
  swapBuffers();
  redraw();
}

void JSBSimVisualizer::keyPressEvent(KeyEvent& event) {
  if (_imgui_ctx.handleKeyPressEvent(event)) return;
  _blackboard->input_blackboard->keys_down.insert(event.key());
  event.setAccepted();
}

void JSBSimVisualizer::keyReleaseEvent(KeyEvent& event) {
  if (_imgui_ctx.handleKeyReleaseEvent(event)) return;
  _blackboard->input_blackboard->keys_down.erase(event.key());
  event.setAccepted();
}

void JSBSimVisualizer::pointerPressEvent(PointerEvent& event) {
  if (_imgui_ctx.handlePointerPressEvent(event)) return;
  
  if (event.pointer() == Sdl2Application::Pointer::MouseLeft) {
    _blackboard->input_blackboard->mouse_held = true;
    event.setAccepted();
  }
}

void JSBSimVisualizer::pointerReleaseEvent(PointerEvent& event) {
  if (_imgui_ctx.handlePointerReleaseEvent(event)) return;

  if (event.pointer() == Sdl2Application::Pointer::MouseLeft) {
    _blackboard->input_blackboard->mouse_held = false;
    event.setAccepted();
  }
}

void JSBSimVisualizer::pointerMoveEvent(PointerMoveEvent& event) {
  if (_blackboard->sim_state_blackboard->cursor_hidden == types::eCursorHidden::VISIBLE) {
    if (_imgui_ctx.handlePointerMoveEvent(event)) return;
  }
  _blackboard->input_blackboard->mouse_position = event.position();
  _blackboard->input_blackboard->mouse_delta = event.relativePosition();
  event.setAccepted();
}

void JSBSimVisualizer::scrollEvent(ScrollEvent& event) {
  // Don't scroll if we are scrolling an ImGui window
  if (_imgui_ctx.handleScrollEvent(event)) {
    event.setAccepted();
    return;
  }
}

void JSBSimVisualizer::viewportEvent(ViewportEvent& event) {
  Magnum::GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});
  _imgui_ctx.relayout(
    Magnum::Vector2{
      event.windowSize()} / event.dpiScaling(),
      event.windowSize(), 
      event.framebufferSize()
    );
}

void JSBSimVisualizer::textInputEvent(TextInputEvent& event) {
  if (_imgui_ctx.handleTextInputEvent(event)) return;
}

