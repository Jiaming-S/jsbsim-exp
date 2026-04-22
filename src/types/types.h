#pragma once

#include <Magnum/GL/Mesh.h>
#include <Magnum/SceneGraph/Object.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>

#include <FGFDMExec.h>

#include <memory>

namespace types {

using Object3D = Magnum::SceneGraph::Object<Magnum::SceneGraph::MatrixTransformation3D>;
using Scene3D = Magnum::SceneGraph::Scene<Magnum::SceneGraph::MatrixTransformation3D>;

enum AircraftType {
  F16,
};

struct AircraftInitialConditionConfig {
  double altitude_asl_ft;
  double true_airspeed_fps;
  double roll_deg;
  double pitch_deg;
  double heading_deg;
  double latitude_deg;
  double longitude_deg;
};

enum AircraftInitialConditionPreset {
  DEFAULT,
  DEFAULT_OPPONENT,
  ON_GROUND,
  TAKEOFF_ROLL,
  TAKEOFF_ROLL_ROTATION,
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

struct ModelPart {
  Magnum::GL::Mesh mesh;
  Magnum::Matrix4 transformation;
};

}
