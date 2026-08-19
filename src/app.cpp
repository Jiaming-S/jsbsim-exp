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
    _aircraft_control_component{_blackboard, this},
    _aircraft_state_info_component{_blackboard, this},
    _camera_control_component{_blackboard, this},
    _camera_draw_component{_blackboard, this},
    _foreign_input_component{_blackboard, this},
    _gui_component{_blackboard, this},
    _hand_of_god_control_component{_blackboard, this},
    _hand_of_god_tick_component{_blackboard, this},
    _keyboard_input_component{_blackboard, this},
    _mouse_cursor_hide_component{_blackboard, this},
    _mouse_input_component{_blackboard, this},
    _scenario_control_component{_blackboard, this},
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

  // Request default debugging scenario
  _blackboard->sim_state_blackboard->scenario_reset_request = types::eScenarioResetRequest::REQUEST_RESET_TO_DEBUG;

  { // Open and bind telemetry/HoG port
    _telemetry_pub_socket = zmq::socket_t{_zmq_ctx, zmq::socket_type::pub};
    _telemetry_pub_socket.bind("tcp://127.0.0.1:5555");
    
    _hand_of_god_sub_socket = zmq::socket_t{_zmq_ctx, zmq::socket_type::sub};
    _hand_of_god_sub_socket.set(zmq::sockopt::subscribe, "");
    _hand_of_god_sub_socket.bind("tcp://*:5556");
  }

  { // Initialize blackboard pointers
    _blackboard->magnum_blackboard->scene_root = new types::Object3D(&_scene);
    _blackboard->magnum_blackboard->imgui_ctx = &_imgui_ctx;
    _blackboard->magnum_blackboard->drawables = &_drawables;
    _blackboard->magnum_blackboard->background_drawables = &_background_drawables;
    _blackboard->magnum_blackboard->model_repo = &_model_repo;
    _blackboard->magnum_blackboard->phong_shader = &_phong_shader;
    _blackboard->magnum_blackboard->floor_shader = &_floor_shader;
    _blackboard->magnum_blackboard->sky_shader = &_sky_shader;
    _blackboard->magnum_blackboard->shadow_shader = &_shadow_shader;
    _blackboard->network_blackboard->zmq_ctx = &_zmq_ctx;
    _blackboard->network_blackboard->telemetry_pub_socket = &_telemetry_pub_socket;
    _blackboard->network_blackboard->hand_of_god_sub_socket = &_hand_of_god_sub_socket;
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
  // Listen for potential scenario reset requests (always runs once on startup)
  _scenario_control_component.handle_dispatch();

  // Tick jsbsim
  _sim_tick_component.handle_dispatch();

  // Tick visuals
  _aircraft_state_info_component.handle_dispatch();
  _vis_tick_component.handle_dispatch();

  // Send telemetry
  _telemetry_tick_component.handle_dispatch();

  // Wait for hand-of-god control (if enabled)
  _hand_of_god_tick_component.handle_dispatch();
  _hand_of_god_control_component.handle_dispatch();
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

