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
#include <initialization/FGTrim.h>
#include <models/FGFCS.h>
#include <models/FGPropulsion.h>

#include <memory>
#include <unordered_map>
#include <vector>
#include <string>

#include "../types/types.h"

namespace utils {

extern void populate_tmp_jsbsim_dir(types::AircraftType aircraft_type = types::AircraftType::F16);

/// @brief Loads and initializes an aircraft FGFDMExec
/// @param aircraft_type 
/// @param aircraft_type_string 
/// @param quiet 
/// @return 
extern std::unique_ptr<JSBSim::FGFDMExec> load_aircraft(
  types::AircraftType aircraft_type = types::AircraftType::F16,
  std::string aircraft_type_string = "f16",
  bool quiet = true
);

/// Convert aircraft type to corresponding type_string
extern std::string to_type_string(types::AircraftType& t);

/// Convert keypoint to coordinate relative to model
extern Magnum::Vector3 to_keypoint_coords(
  types::AircraftType aircraft_type,
  types::AircraftKeyPoints keypoint
);

/// Right Up Back
extern Magnum::Vector3 as_magnum_RUB(float x, float y, float z);

/// North East Down
extern Magnum::Vector3 as_jsbsim_NED(float x, float y, float z);

/// Apply a preset initial condition
extern void apply_preset_ic(types::AircraftInitialConditionPreset preset, JSBSim::FGFDMExec& fdmexec);

/// Apply a preset initial control
extern void apply_preset_controls(types::AircraftInitialConditionPreset preset, JSBSim::FGFDMExec& fdmexec);

}
