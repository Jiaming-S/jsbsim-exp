#pragma once

#include <FGFDMExec.h>
#include <initialization/FGInitialCondition.h>

#include <memory>
#include <string>

/// @brief Loads and initializes an aircraft FGFDMExec
/// @param aircraft_fdmexec 
/// @param aircraft_type_dir 
/// @param aircraft_ic_file 
/// @param quiet 
extern void load_aircraft(
  JSBSim::FGFDMExec &aircraft_fdmexec,
  std::string aircraft_type_dir,
  std::string aircraft_ic_file,
  bool quiet
);

