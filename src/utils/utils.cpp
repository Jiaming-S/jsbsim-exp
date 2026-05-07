#include "utils.h"

#define PUSH_JSBSIM_DEBUG_LEVEL \
  short orig_debug_level = JSBSim::FGJSBBase::debug_lvl; \
  if (quiet) JSBSim::FGJSBBase::debug_lvl = 0; 
#define POP_JSBSIM_DEBUG_LEVEL \
  JSBSim::FGJSBBase::debug_lvl = orig_debug_level;

namespace utils {

void _populate_tmp_jsbsim_dir(
  Corrade::Utility::Resource& _rs,
  types::AircraftType aircraft_type
) {
  std::string tmp_dir = (*Corrade::Utility::Path::temporaryDirectory()) + "/jsbsim-flightmodels/";
  Magnum::Utility::Path::make(tmp_dir);
  Magnum::Utility::Path::make(tmp_dir + "aircraft/");
  Magnum::Utility::Path::make(tmp_dir + "systems/");
  Magnum::Utility::Path::make(tmp_dir + "engine/");

  switch (aircraft_type) {
    case types::AircraftType::F16:
      Magnum::Utility::Path::make(tmp_dir + "aircraft/f16/");

      // Fightmodel
      Magnum::Utility::Path::write(tmp_dir + "aircraft/f16/f16.xml",     _rs.getString("assets/f16/flightmodel/f16.xml"));
      Magnum::Utility::Path::write(tmp_dir + "aircraft/f16/reset00.xml", _rs.getString("assets/f16/flightmodel/reset00.xml"));
      // Systems
      Magnum::Utility::Path::write(tmp_dir + "systems/hook.xml", _rs.getString("assets/f16/flightmodel/hook.xml"));
      Magnum::Utility::Path::write(tmp_dir + "systems/pushback.xml", _rs.getString("assets/f16/flightmodel/pushback.xml"));
      // Engine
      Magnum::Utility::Path::write(tmp_dir + "engine/F100-PW-229.xml", _rs.getString("assets/f16/flightmodel/F100-PW-229.xml"));
      Magnum::Utility::Path::write(tmp_dir + "engine/direct.xml", _rs.getString("assets/f16/flightmodel/direct.xml"));
      break;
    default: break;
  }
}

std::unique_ptr<JSBSim::FGFDMExec> load_aircraft(
  types::AircraftType aircraft_type,
  std::string aircraft_type_string,
  bool quiet
) {
  PUSH_JSBSIM_DEBUG_LEVEL
  std::unique_ptr<JSBSim::FGFDMExec> aircraft_fdmexec = std::make_unique<JSBSim::FGFDMExec>();

  // Load and attach to tmp dir
  std::string tmp_dir = (*Corrade::Utility::Path::temporaryDirectory()) + "/jsbsim-flightmodels/";
  aircraft_fdmexec->SetAircraftPath(SGPath(tmp_dir + "aircraft/"));
  aircraft_fdmexec->SetEnginePath(SGPath(tmp_dir + "engine/"));
  aircraft_fdmexec->SetSystemsPath(SGPath(tmp_dir + "systems/"));

  // Import aircraft (default "f16")
  if (!aircraft_fdmexec->LoadModel(aircraft_type_string)) {
    std::cerr << "Failed to load model \"" << aircraft_type_string << "\" from tmp_dir \`" << tmp_dir << "\`" << std::endl;
    return nullptr;
  }

  POP_JSBSIM_DEBUG_LEVEL
  
  // Return unique_ptr
  return std::move(aircraft_fdmexec);
}

std::string to_type_string(types::AircraftType& t) {
  switch (t) {
    case types::AircraftType::F16: return "f16";
    default: return NULL;
  }
}

Magnum::Vector3 as_magnum_RUB(float x, float y, float z) {
  return Magnum::Vector3{y, -z, -x};
}

Magnum::Vector3 as_jsbsim_NED(float x, float y, float z) {
  return Magnum::Vector3{-z, x, -y};
}

void apply_preset_ic(
  types::AircraftInitialConditionPreset preset,
  JSBSim::FGFDMExec& fdmexec
) {
  switch (preset) {
    case types::AircraftInitialConditionPreset::DEFAULT:
      fdmexec.GetIC()->SetAltitudeASLFtIC(60.0f);
      fdmexec.GetIC()->SetVNorthFpsIC(450.0f);
      fdmexec.GetIC()->SetPhiDegIC(0.0f);
      fdmexec.GetIC()->SetThetaDegIC(0.0f);
      fdmexec.GetIC()->SetPsiDegIC(0.0f);
      fdmexec.GetIC()->SetLatitudeDegIC(0.0f);
      fdmexec.GetIC()->SetLongitudeDegIC(0.0f);
      break;
    case types::AircraftInitialConditionPreset::DEFAULT_OPPONENT:
      fdmexec.GetIC()->SetAltitudeASLFtIC(60.0f);
      fdmexec.GetIC()->SetVNorthFpsIC(450.0f);
      fdmexec.GetIC()->SetPhiDegIC(0.0f);
      fdmexec.GetIC()->SetThetaDegIC(0.0f);
      fdmexec.GetIC()->SetPsiDegIC(180.0f);
      fdmexec.GetIC()->SetLatitudeDegIC(-1e-3f);
      fdmexec.GetIC()->SetLongitudeDegIC(0.0f);
      break;
    case types::AircraftInitialConditionPreset::ON_GROUND:
      fdmexec.GetIC()->SetAltitudeASLFtIC(5.6f);
      fdmexec.GetIC()->SetVNorthFpsIC(0.0f);
      fdmexec.GetIC()->SetPhiDegIC(0.0f);
      fdmexec.GetIC()->SetThetaDegIC(0.0f);
      fdmexec.GetIC()->SetPsiDegIC(0.0f);
      fdmexec.GetIC()->SetLatitudeDegIC(0.0f);
      fdmexec.GetIC()->SetLongitudeDegIC(0.0f);
      break;
    case types::AircraftInitialConditionPreset::TAKEOFF_ROLL:
      fdmexec.GetIC()->SetAltitudeASLFtIC(5.6f);
      fdmexec.GetIC()->SetVNorthFpsIC(20.0f);
      fdmexec.GetIC()->SetPhiDegIC(0.0f);
      fdmexec.GetIC()->SetThetaDegIC(0.0f);
      fdmexec.GetIC()->SetPsiDegIC(0.0f);
      fdmexec.GetIC()->SetLatitudeDegIC(0.0f);
      fdmexec.GetIC()->SetLongitudeDegIC(0.0f);
      break;
    case types::AircraftInitialConditionPreset::TAKEOFF_ROLL_ROTATION:
      fdmexec.GetIC()->SetAltitudeASLFtIC(5.6f);
      fdmexec.GetIC()->SetVNorthFpsIC(200.0f);
      fdmexec.GetIC()->SetPhiDegIC(0.0f);
      fdmexec.GetIC()->SetThetaDegIC(3.0f);
      fdmexec.GetIC()->SetPsiDegIC(0.0f);
      fdmexec.GetIC()->SetLatitudeDegIC(-2e-3f);
      fdmexec.GetIC()->SetLongitudeDegIC(0.0f);
      break;
    case types::AircraftInitialConditionPreset::LEFT_SPIRAL:
      fdmexec.GetIC()->SetAltitudeASLFtIC(200.0f);
      fdmexec.GetIC()->SetVNorthFpsIC(650.0f);
      fdmexec.GetIC()->SetPhiDegIC(45.0f);
      fdmexec.GetIC()->SetThetaDegIC(0.0f);
      fdmexec.GetIC()->SetPsiDegIC(0.0f);
      fdmexec.GetIC()->SetLatitudeDegIC(0.0f);
      fdmexec.GetIC()->SetLongitudeDegIC(2e-3f);
      break;
    case types::AircraftInitialConditionPreset::RIGHT_SPIRAL:
      fdmexec.GetIC()->SetAltitudeASLFtIC(200.0f);
      fdmexec.GetIC()->SetVNorthFpsIC(650.0f);
      fdmexec.GetIC()->SetPhiDegIC(-45.0f);
      fdmexec.GetIC()->SetThetaDegIC(0.0f);
      fdmexec.GetIC()->SetPsiDegIC(0.0f);
      fdmexec.GetIC()->SetLatitudeDegIC(0.0f);
      fdmexec.GetIC()->SetLongitudeDegIC(-2e-3f);
      break;
  }
}

void apply_preset_controls(
  types::AircraftInitialConditionPreset preset,
  JSBSim::FGFDMExec& fdmexec
) {
  switch (preset) {
    case types::AircraftInitialConditionPreset::DEFAULT:
      fdmexec.GetFCS()->SetThrottleCmd(0, 1.0);
      fdmexec.GetPropulsion()->SetEngineRunning(0);
      break;
    case types::AircraftInitialConditionPreset::DEFAULT_OPPONENT:
      fdmexec.GetFCS()->SetThrottleCmd(0, 1.0);
      fdmexec.GetPropulsion()->SetEngineRunning(0);
      break;
    case types::AircraftInitialConditionPreset::ON_GROUND:
      fdmexec.GetFCS()->SetThrottleCmd(0, 0.0);
      fdmexec.GetPropulsion()->SetEngineRunning(0);
      break;
    case types::AircraftInitialConditionPreset::TAKEOFF_ROLL:
      fdmexec.GetFCS()->SetThrottleCmd(0, 1.0);
      fdmexec.GetPropulsion()->SetEngineRunning(0);
      break;
    case types::AircraftInitialConditionPreset::TAKEOFF_ROLL_ROTATION: 
      fdmexec.GetFCS()->SetThrottleCmd(0, 1.0);
      fdmexec.GetFCS()->SetDeCmd(-0.2);
      fdmexec.GetPropulsion()->SetEngineRunning(0);
      break;
    case types::AircraftInitialConditionPreset::LEFT_SPIRAL: 
      fdmexec.GetFCS()->SetThrottleCmd(0, 1.0);
      fdmexec.GetFCS()->SetDeCmd(-0.3);
      // fdmexec.GetFCS()->SetDaCmd(0.12);
      fdmexec.GetFCS()->SetDrCmd(-0.15);
      fdmexec.GetPropulsion()->SetEngineRunning(0);
      break;
    case types::AircraftInitialConditionPreset::RIGHT_SPIRAL: 
      fdmexec.GetFCS()->SetThrottleCmd(0, 1.0);
      fdmexec.GetFCS()->SetDeCmd(-0.3);
      // fdmexec.GetFCS()->SetDaCmd(-0.12);
      fdmexec.GetFCS()->SetDrCmd(0.15);
      fdmexec.GetPropulsion()->SetEngineRunning(0);
      break;
  }
}

}
