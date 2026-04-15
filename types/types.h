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

struct AircraftHandle {
  std::unique_ptr<JSBSim::FGFDMExec> fdmexec;
  types::Object3D *model;
};

struct ModelPart {
  Magnum::GL::Mesh mesh;
  Magnum::Matrix4 transformation;
};

}
