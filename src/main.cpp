// Magnum
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/Math/Color.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Primitives/Cube.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/Object.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/Shaders/PhongGL.h>
#include <Magnum/Trade/MeshData.h>

// JSBSim
#include <FGFDMExec.h>
#include <initialization/FGInitialCondition.h>
#include <models/FGInput.h>

// Standard library
#include <iostream>
#include <memory>
#include <string>

// Project
#include "sim.h"
#include "utils.h"
#include "../types/types.h"

// Literal operators
using namespace Magnum::Math::Literals;


class ColoredDrawable : public Magnum::SceneGraph::Drawable3D {
  // Magnum
  Magnum::Shaders::PhongGL& _shader;
  Magnum::GL::Mesh& _mesh;

  public:
    explicit ColoredDrawable(types::Object3D& object, Magnum::Shaders::PhongGL& shader, Magnum::GL::Mesh& mesh, Magnum::SceneGraph::DrawableGroup3D& group)
      : Magnum::SceneGraph::Drawable3D{object, &group}, _shader(shader), _mesh(mesh) {}

  private:
    void draw(const Magnum::Matrix4& transformationMatrix, Magnum::SceneGraph::Camera3D& camera) override;
};

void ColoredDrawable::draw(const Magnum::Matrix4& transformationMatrix, Magnum::SceneGraph::Camera3D& camera) {
  // Apply camera and object transformations to the shader
  _shader.setLightPositions({{7.0f, 5.0f, 2.5f, 0.0f}})
    .setDiffuseColor(0x3bd267_rgbf)
    .setTransformationMatrix(transformationMatrix)
    .setNormalMatrix(transformationMatrix.normalMatrix())
    .setProjectionMatrix(camera.projectionMatrix());
  _mesh.draw(_shader);
}


class JSBSimVisualizer: public Magnum::Platform::Application {
  public:
    explicit JSBSimVisualizer(const Arguments& arguments);

  private:
    // Overrides
    void tickEvent() override;
    void drawEvent() override;
    void keyPressEvent(KeyEvent& event) override;

    // Magnum
    Magnum::GL::Mesh _mesh;
    Magnum::Shaders::PhongGL _shader;
    Magnum::SceneGraph::DrawableGroup3D _drawables;

    types::Scene3D _scene;

    // Camera
    Magnum::SceneGraph::Camera3D* _camera;
    types::Object3D * _mount;

    // JSBSim
    std::vector<types::AircraftHandle> _aircraft;
};

JSBSimVisualizer::JSBSimVisualizer(const Arguments& arguments)
  : Magnum::Platform::Application{arguments, Configuration{}.setTitle("Visualizer")}
{
  // Enable depth test
  Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::DepthTest);

  // Load meshes and shaders
  this->_mesh = Magnum::MeshTools::compile(Magnum::Primitives::cubeSolid());

  // Load and setup camera
  _mount = new types::Object3D{&_scene};
  _mount->translate(Magnum::Vector3::zAxis(10.0f));

  _camera = new Magnum::SceneGraph::Camera3D{*_mount};
  _camera->setProjectionMatrix(
    Magnum::Matrix4::perspectiveProjection(
      35.0_degf,
      Magnum::Vector2{windowSize()}.aspectRatio(),
      0.01f,
      100.0f
    )
  );

  // Load and setup aircraft
  std::unique_ptr<JSBSim::FGFDMExec> red1 = std::make_unique<JSBSim::FGFDMExec>();
  types::Object3D *red1_aircraft_object = new types::Object3D{&_scene};
  load_aircraft(red1, "f16", "reset00.xml", true);
  new ColoredDrawable{*red1_aircraft_object, _shader, _mesh, _drawables};
  this->_aircraft.push_back(types::AircraftHandle{std::move(red1),  red1_aircraft_object});
  
  std::unique_ptr<JSBSim::FGFDMExec> blue1 = std::make_unique<JSBSim::FGFDMExec>();
  types::Object3D *blue1_aircraft_object = new types::Object3D{&_scene};
  load_aircraft(blue1, "f16", "reset00.xml", true);
  new ColoredDrawable{*blue1_aircraft_object, _shader, _mesh, _drawables};
  this->_aircraft.push_back(types::AircraftHandle{std::move(blue1), blue1_aircraft_object});
}

void JSBSimVisualizer::tickEvent() {
  
}

void JSBSimVisualizer::drawEvent() {
  Magnum::GL::defaultFramebuffer.clear(Magnum::GL::FramebufferClear::Color | Magnum::GL::FramebufferClear::Depth);

  _camera->draw(_drawables);
  
  swapBuffers();
  redraw();
}

void JSBSimVisualizer::keyPressEvent(KeyEvent& event) {
  const float moveSpeed = 0.5f;
  const auto rotSpeed = 5.0_degf;

  switch(event.key()) {
    // Translation (Move Camera)
    case KeyEvent::Key::W: _mount->translateLocal(Magnum::Vector3::zAxis(-moveSpeed)); break;
    case KeyEvent::Key::S: _mount->translateLocal(Magnum::Vector3::zAxis( moveSpeed)); break;
    case KeyEvent::Key::A: _mount->translateLocal(Magnum::Vector3::xAxis(-moveSpeed)); break;
    case KeyEvent::Key::D: _mount->translateLocal(Magnum::Vector3::xAxis( moveSpeed)); break;
    case KeyEvent::Key::Space:     _mount->translateLocal(Magnum::Vector3::yAxis( moveSpeed)); break;
    case KeyEvent::Key::LeftShift: _mount->translateLocal(Magnum::Vector3::yAxis(-moveSpeed)); break;
    
    // Rotation (Pitch and Yaw Camera)
    case KeyEvent::Key::Up:   _mount->rotate( rotSpeed, Magnum::Vector3::xAxis()); break;
    case KeyEvent::Key::Down: _mount->rotate(-rotSpeed, Magnum::Vector3::xAxis()); break;
    case KeyEvent::Key::Q: _mount->rotate( rotSpeed, Magnum::Vector3::yAxis()); break;
    case KeyEvent::Key::E: _mount->rotate(-rotSpeed, Magnum::Vector3::yAxis()); break;
    
    // Roll Camera
    case KeyEvent::Key::Left: _mount->rotate( rotSpeed, Magnum::Vector3::zAxis()); break;
    case KeyEvent::Key::Right:_mount->rotate(-rotSpeed, Magnum::Vector3::zAxis()); break;
    
    default: return;
  }
  
  event.setAccepted();
}

int main(int argc, char** argv) {
  Magnum::Platform::Application::Arguments args{argc, argv};
  JSBSimVisualizer app{args};
  return app.exec();
}
