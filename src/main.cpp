// Magnum
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Shaders/VertexColorGL.h>

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


class JSBSimVis: public Magnum::Platform::Application {
  public:
    explicit JSBSimVis(const Arguments& arguments);

  private:
    // Overloads
    void tickEvent() override;
    void drawEvent() override;

    // Magnum
    Magnum::GL::Mesh _mesh;
    Magnum::Shaders::VertexColorGL2D _shader;

    // JSBSim
    std::vector<std::unique_ptr<JSBSim::FGFDMExec>> _aircraft;
};

JSBSimVis::JSBSimVis(const Arguments& arguments):
  Magnum::Platform::Application{arguments, Configuration{}.setTitle("Visualizer")},
  _mesh{},
  _shader{},
  _aircraft{}
{
  std::unique_ptr<JSBSim::FGFDMExec> red1 = std::make_unique<JSBSim::FGFDMExec>();
  load_aircraft(*red1, "f16", "reset00.xml", true);
  
  std::unique_ptr<JSBSim::FGFDMExec> blue1 = std::make_unique<JSBSim::FGFDMExec>();
  load_aircraft(*blue1, "f16", "reset00.xml", true);

  this->_aircraft.push_back(std::move(red1));
  this->_aircraft.push_back(std::move(blue1));
}

void JSBSimVis::tickEvent() {
  
}

void JSBSimVis::drawEvent() {
  Magnum::GL::defaultFramebuffer.clear(Magnum::GL::FramebufferClear::Color);


  
  swapBuffers();
}

int main(int argc, char** argv) {
  Magnum::Platform::Application::Arguments args{argc, argv};
  JSBSimVis app{args};
  return app.exec();
}
