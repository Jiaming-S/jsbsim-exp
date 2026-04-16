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

enum AircraftInitialConditionType {
  DEFAULT,
  DEFAULT_OPPONENT,
  ON_GROUND,
  CUSTOM
};

struct AircraftInitialConditionConfig {
  double altitude_agl_ft;
  double true_airspeed_fps;
  double roll_deg;
  double pitch_deg;
  double heading_deg;
};

struct ModelPart {
  Magnum::GL::Mesh mesh;
  Magnum::Matrix4 transformation;
};

}
