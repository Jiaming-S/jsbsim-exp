#pragma once

#include "Magnum/Magnum.h"
#include <Magnum/GL/Mesh.h>
#include <Magnum/SceneGraph/Object.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>

#include <FGFDMExec.h>


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
  F16_NO_PID,
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

struct AircraftTrailBreadcrumb {
  AircraftStateInfo breadcrumb;
  double sim_time;
};

enum AircraftKeyPoints {
  NOSE,
  WINGTIP_L,
  WINGTIP_R,
  ENGINE_EXHAUST,
};


// Blackboard Types

// Info on our physics simulation type
enum eSimPhysicsState {
  NORMAL,
  PAUSED,
  CUSTOM,
};

// Info on whether we are controlling the camera or model
enum eSimControlType {
  CAMERA,
  MODEL,
};

// Info on whether we are orientation-locked or free floating about camera root
enum eCameraType {
  LOCKED,
  FREE,
};

// Info on whether there is an active aircraft
enum eAircraftActive {
  NO_ACTIVE,
  HAS_ACTIVE,
};

enum eCursorHidden {
  HIDDEN_AND_LOCKED,
  VISIBLE,
};

}
