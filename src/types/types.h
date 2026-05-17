#pragma once

#include <Magnum/GL/Mesh.h>
#include <Magnum/SceneGraph/Object.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>

#include <FGFDMExec.h>

#include <memory>

namespace types {

using Object3D = Magnum::SceneGraph::Object<Magnum::SceneGraph::MatrixTransformation3D>;
using Scene3D = Magnum::SceneGraph::Scene<Magnum::SceneGraph::MatrixTransformation3D>;

struct Object3DRenderable {
  types::Object3D *node;
  Magnum::GL::Mesh *mesh;
  Magnum::GL::Texture2D* texture;
};

enum AircraftType {
  F16,
};

enum AircraftInitialConditionPreset {
  DEFAULT,
  DEFAULT_OPPONENT,
  ON_GROUND,
  TAKEOFF_ROLL,
  TAKEOFF_ROLL_ROTATION,
  LEFT_SPIRAL,
  RIGHT_SPIRAL,
  LEFT_TAXI,
  RIGHT_TAXI,
};

struct AircraftStateInfo {
  Magnum::Rad pitch;
  Magnum::Rad roll;
  Magnum::Rad yaw;
  double alt;
  double north;
  double east;
  double down;
  double v_north;
  double v_east;
  double v_down;
};

enum AircraftKeyPoints {
  NOSE,
  LANDING_GEAR_BOTTOM,
  WINGTIP_L,
  WINGTIP_R,
};

struct SimContext {
  enum State {
    NORMAL,
    PAUSED,
    CUSTOM,
  };

  enum CameraType {
    FREE,
    LOCKED,
  };

  enum CameraPosition {
    SPECTATOR,
    THIRDPERSON,
    COCKPIT,
  };

  enum ControlType {
    CAMERA,
    MODEL,
  };
  
  SimContext::State state;
  SimContext::CameraType camera_type;
  SimContext::ControlType control_type;
  SimContext::CameraPosition camera_pos;
  size_t _active_aircraft_index;
};

}
