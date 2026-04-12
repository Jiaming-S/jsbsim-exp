#include <Magnum/SceneGraph/Object.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>

#include <FGFDMExec.h>

#include <memory>

typedef Magnum::SceneGraph::Object<Magnum::SceneGraph::MatrixTransformation3D> Object3D;
typedef Magnum::SceneGraph::Scene<Magnum::SceneGraph::MatrixTransformation3D> Scene3D;

struct AircraftHandle {
  std::unique_ptr<JSBSim::FGFDMExec> fdmexec;
  Magnum::SceneGraph::Object<Magnum::SceneGraph::MatrixTransformation3D> *model;
};
