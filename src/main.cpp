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
#include "coloreddrawable.h"
#include "sim.h"
#include "gui/gui.h"
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

    // Private Methods
    void _moveCameraMount();

    // Magnum
    Magnum::Shaders::PhongGL _shader;
    Magnum::SceneGraph::DrawableGroup3D _drawables;
    std::unordered_map<std::string, Magnum::GL::Mesh> _meshes;
    std::unordered_map<std::string, std::vector<types::ModelPart>> _aircraft_part_meshes;

    Magnum::Timeline _timeline;
    types::Scene3D _scene;

    // Input
    std::unordered_map<Sdl2Application::Key, bool> _keys_down;

    // Camera
    types::Object3D *_mount, *_revolut;
    Magnum::SceneGraph::Camera3D *_camera;

    // Resource manager
    Corrade::Utility::Resource _rs{"assets"};

    // ImGui
    Magnum::ImGuiIntegration::Context _imgui{Magnum::NoCreate};

    // JSBSim
    std::vector<AircraftHandle> _aircraft;
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
  
  // Enable draw lines between polygons
  // Magnum::GL::Renderer::setPolygonMode(Magnum::GL::Renderer::PolygonMode::Line);

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

  // Load meshes and shaders
  std::vector<std::pair<std::string, std::string>> to_import = {
    {"f16", "assets/f16/f16.glb"},
  };
  
  utils::load_meshes(_rs, _aircraft_part_meshes, to_import);

  // Add floor
  types::Object3D *floor = new types::Object3D{&_scene};
  floor->scale(Magnum::Vector3{100.0f, 0.01f, 100.0f})
    .translateLocal(Magnum::Vector3{0.0f, -0.1f, 0.0f})
    .rotateXLocal(1.5707_radf);
  _meshes["floor_mesh"] = Magnum::MeshTools::compile(Magnum::Primitives::grid3DWireframe({100, 100}));
  new ColoredDrawable{*floor, _shader, _meshes["floor_mesh"], _drawables};

  // Load and setup camera
  _mount = new types::Object3D{&_scene};
  _revolut = new types::Object3D{_mount};

  _camera = new Magnum::SceneGraph::Camera3D{*_revolut};
  _camera->setProjectionMatrix(
    Magnum::Matrix4::perspectiveProjection(
      90.0_degf,
      Magnum::Vector2{windowSize()}.aspectRatio(),
      0.01f,
      INFINITY
    )
  );

  // Load and setup aircraft
  utils::_construct_tmp_jsbsim_dir(_rs, types::AircraftType::F16);

  std::vector<types::AircraftInitialConditionPreset> presets = {
    types::AircraftInitialConditionPreset::DEFAULT,
    types::AircraftInitialConditionPreset::DEFAULT_OPPONENT,
    types::AircraftInitialConditionPreset::ON_GROUND,
    types::AircraftInitialConditionPreset::TAKEOFF_ROLL,
    types::AircraftInitialConditionPreset::TAKEOFF_ROLL_ROTATION,
  };

  for (auto preset : presets) {
    AircraftHandle aircraft = AircraftHandle{types::AircraftType::F16};
    aircraft.with_fdmexec()
      .with_ic(utils::fetch_preset(preset))
      .with_model(new types::Object3D{&_scene})
      .with_meshes(_aircraft_part_meshes)
      .link(_shader, _drawables);

    aircraft._fdmexec->GetPropertyManager()->GetNode("fcs/throttle-cmd-norm[0]")->setDoubleValue(1.0);
    aircraft._fdmexec->GetPropertyManager()->GetNode("propulsion/engine[0]/set-running")->setIntValue(1);
    _aircraft.push_back(std::move(aircraft));
  }

  // Start Magnum timeline
  _timeline.start();
  
  // Enable VSync
  setSwapInterval(1);
}

// Tick Controller and Update Model
void JSBSimVisualizer::tickEvent() {
  for (auto& cur : _aircraft) {
    // Tick Controller
    cur.update_sim();
    // Update Model
    cur.update_model();
  }
}

void JSBSimVisualizer::_moveCameraMount() {
  float speed = 0.1f;
  auto speed_rotation = 1.0_degf;

  if(_keys_down[Sdl2Application::Key::LeftShift]) speed *= 5.0f;

  if(_keys_down[Sdl2Application::Key::Up])    _revolut->rotateLocal( speed_rotation, Magnum::Vector3::xAxis());
  if(_keys_down[Sdl2Application::Key::Down])  _revolut->rotateLocal(-speed_rotation, Magnum::Vector3::xAxis());
  
  if(_keys_down[Sdl2Application::Key::Left]) {
    Magnum::Matrix4 rot = Magnum::Matrix4::rotation(speed_rotation, Magnum::Vector3::yAxis());
    _revolut->setTransformation(rot * _revolut->transformation());
  }

  if(_keys_down[Sdl2Application::Key::Right]) {
    Magnum::Matrix4 rot = Magnum::Matrix4::rotation(-speed_rotation, Magnum::Vector3::yAxis());
    _revolut->setTransformation(rot * _revolut->transformation());
  }

  Magnum::Vector3 forward = _revolut->transformation().backward();
  Magnum::Vector3 right   = _revolut->transformation().right();

  if(_keys_down[Sdl2Application::Key::W]) _mount->translate(forward * -speed);
  if(_keys_down[Sdl2Application::Key::S]) _mount->translate(forward *  speed);
  if(_keys_down[Sdl2Application::Key::A]) _mount->translate(right * -speed);
  if(_keys_down[Sdl2Application::Key::D]) _mount->translate(right *  speed);
  
  if(_keys_down[Sdl2Application::Key::Space])     _mount->translate(Magnum::Vector3::yAxis( speed));
  if(_keys_down[Sdl2Application::Key::LeftCtrl])  _mount->translate(Magnum::Vector3::yAxis(-speed));
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

  // Update camera
  _moveCameraMount();

  // Do draw
  _camera->draw(_drawables);
  

  // Push ImGui required settings
  Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::Blending);
  Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::ScissorTest);
  Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::FaceCulling);
  Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::DepthTest);

  // Draw ImGui
  gui::gui_aircraft(_aircraft);
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
  if(_imgui.handleKeyPressEvent(event)) return;
  _keys_down[event.key()] = true;
  event.setAccepted();
}

void JSBSimVisualizer::keyReleaseEvent(KeyEvent& event) {
  if(_imgui.handleKeyReleaseEvent(event)) return;
  _keys_down[event.key()] = false;
  event.setAccepted();
}

void JSBSimVisualizer::pointerPressEvent(PointerEvent& event) {
  if(_imgui.handlePointerPressEvent(event)) return;
}

void JSBSimVisualizer::pointerReleaseEvent(PointerEvent& event) {
  if(_imgui.handlePointerReleaseEvent(event)) return;
}

void JSBSimVisualizer::pointerMoveEvent(PointerMoveEvent& event) {
  if(_imgui.handlePointerMoveEvent(event)) return;
}

int main(int argc, char** argv) {
  Magnum::Platform::Application::Arguments args{argc, argv};
  JSBSimVisualizer app{args};
  return app.exec();
}
