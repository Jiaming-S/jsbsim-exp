#pragma once

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
#include <memory>
#include <vector>

// Project
#include "blackboard/jsbsimexpblackboard.h"

#include "components/aircraft_control_component.h"
#include "components/aircraft_state_info_component.h"
#include "components/camera_control_component.h"
#include "components/camera_draw_component.h"
#include "components/gui_component.h"
#include "components/foreign_input_component.h"
#include "components/keyboard_input_component.h"
#include "components/mouse_cursor_hide_component.h"
#include "components/mouse_input_component.h"
#include "components/sim_tick_component.h"
#include "components/telemetry_tick_component.h"
#include "components/vis_tick_component.h"

#include "aircrafthandle.h"
#include "camerahandle.h"
#include "drawn/atmospheredrawable.h"
#include "drawn/environmentdrawable.h"
#include "gui/gui.h"
#include "model/model.h"
#include "shaders/floorshader.h"
#include "shaders/skyshader.h"
#include "shaders/shadowshader.h"
#include "types/types.h"

// Literal operators
using namespace Magnum::Math::Literals;


class JSBSimVisualizer : public Magnum::Platform::Application {
  public:
    explicit JSBSimVisualizer(const Arguments& arguments);

  private:    
    // Blackboard Table
    std::shared_ptr<JSBSimExpBlackboard> _blackboard;

    // Blackboard Components
    AircraftControlComponent _aircraft_control_component;
    AircraftStateInfoComponent _aircraft_state_info_component;
    CameraControlComponent _camera_control_component;
    CameraDrawComponent _camera_draw_component;
    ForeignInputComponent _foreign_input_component;
    GuiComponent _gui_component;
    KeyboardInputComponent _keyboard_input_component;
    MouseCursorHideComponent _mouse_cursor_hide_component;
    MouseInputComponent _mouse_input_component;
    SimTickComponent _sim_tick_component;
    TelemetryTickComponent _telemetry_tick_component;
    VisTickComponent _vis_tick_component;

    // Magnum
    Magnum::Shaders::PhongGL _phong_shader;
    
    shaders::FloorShader _floor_shader;
    shaders::SkyShader _sky_shader;
    shaders::ShadowShader _shadow_shader;

    Magnum::SceneGraph::DrawableGroup3D _background_drawables;
    Magnum::SceneGraph::DrawableGroup3D _drawables;

    Magnum::Timeline _timeline;
    types::Scene3D _scene;

    // Environment
    drawn::EnvironmentDrawable* _environment;
    drawn::AtmosphereDrawable* _atmosphere;

    // 3D Model Repository
    model::GLBModelRepository _model_repo;

    // ImGui
    Magnum::ImGuiIntegration::Context _imgui_ctx{Magnum::NoCreate};

    // Overridden Methods
    void tickEvent() override;
    void drawEvent() override;
    void keyPressEvent(KeyEvent& event) override;
    void keyReleaseEvent(KeyEvent& event) override;
    void pointerPressEvent(PointerEvent& event) override;
    void pointerReleaseEvent(PointerEvent& event) override;
    void pointerMoveEvent(PointerMoveEvent& event) override;
    void scrollEvent(ScrollEvent& event) override;
    void viewportEvent(ViewportEvent& event) override;
    void textInputEvent(TextInputEvent& event) override;
};
