// Magnum
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/Math/Color.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/MeshTools/Copy.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Primitives/Cube.h>
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
#include "utils/utils.h"
#include "types/types.h"

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

    // Private Methods
    void _moveCameraMount();

    // Magnum
    Magnum::Shaders::PhongGL _shader;
    Magnum::SceneGraph::DrawableGroup3D _drawables;
    std::unordered_map<std::string, std::vector<types::ModelPart>> _meshes;

    Magnum::Timeline _timeline;
    types::Scene3D _scene;

    // Input
    std::unordered_map<Sdl2Application::Key, bool> _keys_down;

    // Camera
    types::Object3D *_mount, *_revolut;
    Magnum::SceneGraph::Camera3D *_camera;

    // Resource manager
    Corrade::Utility::Resource _rs{"assets"};

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

  // Load meshes and shaders
  std::vector<std::pair<std::string, std::string>> to_import = {
    {"f16", "assets/f16/f16.glb"},
  };
  
  utils::load_meshes(_rs, _meshes, to_import);

  // Load and setup camera
  _mount = new types::Object3D{&_scene};
  _revolut = new types::Object3D{_mount};

  _camera = new Magnum::SceneGraph::Camera3D{*_revolut};
  _camera->setProjectionMatrix(
    Magnum::Matrix4::perspectiveProjection(
      35.0_degf,
      Magnum::Vector2{windowSize()}.aspectRatio(),
      0.01f,
      INFINITY
    )
  );

  // Load and setup aircraft
  utils::_construct_tmp_jsbsim_dir(_rs, types::AircraftType::F16);

  auto red1_config = types::AircraftInitialConditionConfig{};
  auto blue1_config = types::AircraftInitialConditionConfig{};

  AircraftHandle red1 = AircraftHandle{types::AircraftType::F16};
  red1.with_fdmexec()
    .with_ic(red1_config)
    .with_model(new types::Object3D{&_scene})
    .with_meshes(_meshes)
    .link(_shader, _drawables);
  _aircraft.push_back(std::move(red1));

  AircraftHandle blue1 = AircraftHandle{types::AircraftType::F16};
  blue1.with_fdmexec()
    .with_ic(blue1_config)
    .with_model(new types::Object3D{&_scene})
    .with_meshes(_meshes)
    .link(_shader, _drawables);
  _aircraft.push_back(std::move(blue1));

  // Start Magnum timeline
  _timeline.start();
  // Enable VSync
  setSwapInterval(1);
}

void JSBSimVisualizer::tickEvent() {
  for (auto& cur : _aircraft) {
    cur._fdmexec->Run();
  }
}

void JSBSimVisualizer::_moveCameraMount() {
  const float speed = 0.1f;
  const auto speed_rotation = 1.0_degf;

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
  if(_keys_down[Sdl2Application::Key::LeftShift]) _mount->translate(Magnum::Vector3::yAxis(-speed));
}

void JSBSimVisualizer::drawEvent() {
  _timeline.nextFrame();
  Magnum::GL::defaultFramebuffer.clear(Magnum::GL::FramebufferClear::Color | Magnum::GL::FramebufferClear::Depth);

  for (auto& cur : _aircraft) {
    std::shared_ptr<JSBSim::FGAircraft>  cur_aircraft =  cur._fdmexec->GetAircraft();
    std::shared_ptr<JSBSim::FGPropagate> cur_propagate = cur._fdmexec->GetPropagate();

    JSBSim::FGColumnVector3 euler = cur_propagate->GetEuler();
    auto pitch = Magnum::Rad(euler.Entry(1));
    auto roll  = Magnum::Rad(euler.Entry(2));
    auto yaw   = Magnum::Rad(euler.Entry(3));

    double cur_alt = cur_propagate->GetAltitudeASL();
    double radius  = cur_propagate->GetRadius();

    double starting_lat_rad  = 0 * M_PI / 180.0;
    double starting_long_rad = 0 * M_PI / 180.0;

    double dlat_rad = (cur_propagate->GetLatitude() - starting_lat_rad);
    double dlon_rad = (cur_propagate->GetLongitude() - starting_long_rad);

    double north = dlat_rad * radius;
    double east  = dlon_rad * radius * std::cos(cur_propagate->GetLatitude());
    double down  = 0 - cur_alt;

    cur._model->setTransformation(Magnum::Matrix4::translation(utils::as_magnum_RUB(north, east, down)))
      .rotateY(-yaw)
      .rotateX(pitch)
      .rotateZ(roll);
  }

  _moveCameraMount();
  _camera->draw(_drawables);
  
  swapBuffers();
  redraw();
}

void JSBSimVisualizer::keyPressEvent(KeyEvent& event) {
  _keys_down[event.key()] = true;
  event.setAccepted();
}

void JSBSimVisualizer::keyReleaseEvent(KeyEvent& event) {
  _keys_down[event.key()] = false;
  event.setAccepted();
}

int main(int argc, char** argv) {
  Magnum::Platform::Application::Arguments args{argc, argv};
  JSBSimVisualizer app{args};
  return app.exec();
}
