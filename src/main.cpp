// Magnum
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/Math/Color.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/MeshTools/Copy.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Primitives/Cube.h>
#include <Magnum/Primitives/Grid.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/Object.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/Shaders/PhongGL.h>
#include <Magnum/Trade/MeshData.h>
#include <Magnum/Timeline.h>
#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/SceneData.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/ImGuiIntegration/Context.hpp>
#include <Corrade/Utility/Resource.h>

// JSBSim
#include <FGFDMExec.h>
#include <initialization/FGInitialCondition.h>
#include <models/FGAircraft.h>
#include <models/FGInput.h>
#include <models/FGPropagate.h>

// Standard library
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// Project
#include "aircrafthandle.h"
#include "camerahandle.h"
#include "gui/gui.h"
#include "input/input.h"
#include "model/model.h"
#include "model/coloreddrawable.h"
#include "model/textureddrawable.h"
#include "types/types.h"
#include "utils/utils.h"

// Literal operators
using namespace Magnum::Math::Literals;


class JSBSimVisualizer: public Magnum::Platform::Application {
  public:
    explicit JSBSimVisualizer(const Arguments& arguments);

  private:
    // Overrides
    void tickEvent() override;
    void drawEvent() override;
    void keyPressEvent(KeyEvent& event) override;
    void keyReleaseEvent(KeyEvent& event) override;
    void pointerPressEvent(PointerEvent& event) override;
    void pointerReleaseEvent(PointerEvent& event) override;
    void pointerMoveEvent(PointerMoveEvent& event) override;
    void scrollEvent(ScrollEvent& event) override;

    // Magnum
    Magnum::Shaders::PhongGL _shader;
    Magnum::SceneGraph::DrawableGroup3D _drawables;

    Magnum::GL::Mesh _floor_mesh;

    Magnum::Timeline _timeline;
    types::Scene3D _scene;

    // Meta
    std::shared_ptr<types::SimContext> _sim_context;

    // Resource manager
    Corrade::Utility::Resource _rs{"assets"};

    // 3D Model Repository
    model::ModelRepository _model_repo;

    // Input
    input::GlobalInputHandler _input_handler;

    // ImGui
    Magnum::ImGuiIntegration::Context _imgui{Magnum::NoCreate};

    // Camera
    std::unique_ptr<CameraHandle> _cam;

    // JSBSim
    std::vector<AircraftHandle> _aircraft;
    size_t _active_aircraft_index = 0; // todo: alternative datatype
};

JSBSimVisualizer::JSBSimVisualizer(const Arguments& arguments)
  : Magnum::Platform::Application{
      arguments,
      Configuration{}
        .setTitle("Visualizer")
        .addWindowFlags(Magnum::Platform::Sdl2Application::Configuration::WindowFlag::Resizable)
        .addWindowFlags(Magnum::Platform::Sdl2Application::Configuration::WindowFlag::Maximized)
    }
{
  // Enable depth test
  Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::DepthTest);

  // Initialize Magnum shader
  _shader = Magnum::Shaders::PhongGL {
    Magnum::Shaders::PhongGL::Configuration{}.setFlags(Magnum::Shaders::PhongGL::Flag::DiffuseTexture)
  };

  // Initialize ImGui
  _imgui = Magnum::ImGuiIntegration::Context(
    Magnum::Vector2{windowSize()} / dpiScaling(),
    windowSize(),
    framebufferSize()
  );

  // Disable ImGui imgui.ini file
  ImGui::GetIO().IniFilename = nullptr;

  // Enable ImGui blend equations for text
  Magnum::GL::Renderer::setBlendEquation(Magnum::GL::Renderer::BlendEquation::Add, Magnum::GL::Renderer::BlendEquation::Add);
  Magnum::GL::Renderer::setBlendFunction(Magnum::GL::Renderer::BlendFunction::SourceAlpha, Magnum::GL::Renderer::BlendFunction::OneMinusSourceAlpha);

  // Initialize simulation app context
  _sim_context = std::make_shared<types::SimContext>();
  
  // Use standard 1x speed 
  _sim_context->state = types::SimContext::State::NORMAL;
  // Free camera (bound to &scene) 
  _sim_context->camera_type = types::SimContext::CameraType::FREE;
  // Input controls camera (not the JSBSim flight model) 
  _sim_context->control_type = types::SimContext::ControlType::CAMERA;

  // Load and ingest GLTF models
  std::vector<std::pair<std::string, std::string>> models_to_import = {
    {"f16", "assets/f16/f16.glb"},
  };

  for (auto& p : models_to_import) {
    std::string asset_name = p.first;
    std::string asset_filepath = p.second;
    _model_repo.ingest_asset_glb(_rs, asset_name, asset_filepath);
  }

  // Add floor
  types::Object3D *floor = new types::Object3D{&_scene};
  floor->scale(Magnum::Vector3{10000.0f, 0.01f, 10000.0f})
    .translateLocal(Magnum::Vector3{0.0f, -0.1f, 0.0f})
    .rotateXLocal(1.5707_radf);
  _floor_mesh = Magnum::MeshTools::compile(Magnum::Primitives::grid3DWireframe({1000, 1000}));
  new model::ColoredDrawable{*floor, _shader, _floor_mesh, _drawables};

  // Load and setup camera
  _cam = std::make_unique<CameraHandle>(
    &_scene,
    Magnum::Matrix4::perspectiveProjection(
      90.0_degf,
      Magnum::Vector2{windowSize()}.aspectRatio(),
      0.01f,
      INFINITY
    )
  );

  // Load and populate aircraft configs
  utils::populate_tmp_jsbsim_dir(_rs, types::AircraftType::F16);

  // Load initial conditions
  std::vector<types::AircraftInitialConditionPreset> presets = {
    types::AircraftInitialConditionPreset::DEFAULT,
    types::AircraftInitialConditionPreset::DEFAULT_OPPONENT,
    types::AircraftInitialConditionPreset::ON_GROUND,
    types::AircraftInitialConditionPreset::TAKEOFF_ROLL,
    types::AircraftInitialConditionPreset::TAKEOFF_ROLL_ROTATION,
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
      .with_model(_model_repo.get_aircraft_model(types::AircraftType::F16))
      .link(_shader, _drawables);
    _aircraft.push_back(std::move(aircraft));
  }

  // Start Magnum timeline
  _timeline.start();
  
  // Enable VSync
  setSwapInterval(1);
}

// Tick Controller and Update Visual Model
void JSBSimVisualizer::tickEvent() {
  for (auto& cur : _aircraft) {
    // Tick Controller
    if (_sim_context->state != types::SimContext::State::PAUSED) cur.update_sim();
    // Update Visual Model
    cur.update_vis();
  }
}

// Update View
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
  input::CommandedMovement commanded_movement = _input_handler.get_commanded_movement();
  if (_sim_context->control_type == types::SimContext::CAMERA) {
    _cam->apply_commanded_movement(commanded_movement);
  }

  if (_sim_context->control_type == types::SimContext::MODEL)  {
    _aircraft[_active_aircraft_index].apply_commanded_movement(commanded_movement);
  }

  // Do camera draw
  // TODO: make a method for this
  _cam->_camera->draw(_drawables);

  // Push ImGui required settings
  Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::Blending);
  Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::ScissorTest);
  Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::FaceCulling);
  Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::DepthTest);

  // Draw ImGui
  gui::gui_aircraft_debug(_aircraft);
  gui::gui_camera_selection(_aircraft, *_cam, _scene, *_sim_context, _active_aircraft_index);
  gui::gui_input_and_control(_sim_context, commanded_movement);
  gui::gui_hud(_sim_context, commanded_movement);
  _imgui.updateApplicationCursor(*this);
  _imgui.drawFrame();

  // Pop ImGui required settings
  Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::DepthTest);
  Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::FaceCulling);
  Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::ScissorTest);
  Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::Blending);

  // Next
  swapBuffers();
  redraw();
}

void JSBSimVisualizer::keyPressEvent(KeyEvent& event) {
  if (_imgui.handleKeyPressEvent(event)) return;
  _input_handler.handle_key_press_event(event);

  // Update sim state (paused / normal / custom time delta)
  _input_handler.mutate_sim_state(_sim_context);

  event.setAccepted();
}

void JSBSimVisualizer::keyReleaseEvent(KeyEvent& event) {
  if (_imgui.handleKeyReleaseEvent(event)) return;
  _input_handler.handle_key_release_event(event);
  event.setAccepted();
}

void JSBSimVisualizer::pointerPressEvent(PointerEvent& event) {
  if (_imgui.handlePointerPressEvent(event)) return;
  
  if (event.pointer() == Sdl2Application::Pointer::MouseLeft) {
    _input_handler.handle_pointer_press_event(event);
    event.setAccepted();
  }
}

void JSBSimVisualizer::pointerReleaseEvent(PointerEvent& event) {
  if (_imgui.handlePointerReleaseEvent(event)) return;

  if (event.pointer() == Sdl2Application::Pointer::MouseLeft) {
    _input_handler.handle_pointer_release_event(event);
    event.setAccepted();
  }
}

void JSBSimVisualizer::pointerMoveEvent(PointerMoveEvent& event) {
  if (_imgui.handlePointerMoveEvent(event)) return;
  _input_handler.handle_pointer_move_event(event);
  event.setAccepted();
}

void JSBSimVisualizer::scrollEvent(ScrollEvent& event) {
  // Don't scroll if we are scrolling an ImGui window
  if (_imgui.handleScrollEvent(event)) {
    event.setAccepted();
    return;
  }
}

int main(int argc, char** argv) {
  Magnum::Platform::Application::Arguments args{argc, argv};
  JSBSimVisualizer app{args};
  return app.exec();
}
