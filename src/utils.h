#pragma once

#include <Magnum/GL/Mesh.h>
#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/MeshData.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Trade/SceneData.h>
#include <Magnum/Trade/ObjectData3D.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Utility/Debug.h>
#include <Corrade/Utility/Path.h>
#include <Corrade/Utility/Resource.h>
#include <FGFDMExec.h>
#include <initialization/FGInitialCondition.h>

#include <memory>
#include <unordered_map>
#include <vector>
#include <string>

#include "../types/types.h"

namespace utils {

extern void _construct_tmp_jsbsim_dir(
  Corrade::Utility::Resource& _rs,
  types::AircraftType aircraft_type = types::AircraftType::F16
);

/// @brief Loads and initializes an aircraft FGFDMExec
/// @param aircraft_fdmexec 
/// @param aircraft_type_dir 
/// @param aircraft_ic_file 
/// @param quiet 
extern void load_aircraft(
  std::unique_ptr<JSBSim::FGFDMExec>& aircraft_fdmexec,
  types::AircraftType aircraft_type = types::AircraftType::F16,
  bool quiet = true
);

/// @brief Loads all meshes of a gltf as ModelParts into a vector, then publishes to shared _meshes hashmap
/// @param _meshes 
/// @param to_import 
extern void load_meshes(
  Corrade::Utility::Resource& _rs,
  std::unordered_map<std::string, std::vector<types::ModelPart>>& _meshes,
  const std::vector<std::pair<std::string, std::string>>& to_import
);

/// Right Up Back
extern Magnum::Vector3 as_magnum_RUB(float x, float y, float z);

/// North East Down
extern Magnum::Vector3 as_jsbsim_NED(float x, float y, float z);

}
