#include "utils.h"

void load_aircraft(
  std::unique_ptr<JSBSim::FGFDMExec> &aircraft_fdmexec,
  std::string aircraft_type_dir,
  std::string aircraft_ic_file,
  bool quiet
) {
  // Store original debug level
  short orig_debug_level = JSBSim::FGJSBBase::debug_lvl;
  if (quiet) JSBSim::FGJSBBase::debug_lvl = 0; 
  
  // Import aircraft type (default "f16")
  aircraft_fdmexec->SetAircraftPath(SGPath("data/aircraft"));
  aircraft_fdmexec->SetEnginePath(SGPath("data/engine"));
  aircraft_fdmexec->SetSystemsPath(SGPath("data/systems"));
  aircraft_fdmexec->SetOutputPath(SGPath("output"));
  aircraft_fdmexec->LoadModel(aircraft_type_dir);

  // Import aircraft initial conditions (default "reset00.xml")
  std::shared_ptr<JSBSim::FGInitialCondition> aircraft_ic = aircraft_fdmexec->GetIC();
  aircraft_ic->InitializeIC();
  aircraft_ic->Load(SGPath(aircraft_ic_file));

  // Load initial conditions
  aircraft_fdmexec->RunIC();

  // Restore original debug level
  JSBSim::FGJSBBase::debug_lvl = orig_debug_level;
}
