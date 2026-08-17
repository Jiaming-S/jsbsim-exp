#pragma once

#include <memory>

#include "aircraftblackboard.h"
#include "camerablackboard.h"
#include "inputblackboard.h"
#include "magnumblackboard.h"
#include "simstateblackboard.h"
#include "telemetryblackboard.h"

class JSBSimExpBlackboard : public Blackboard {
  public:
    JSBSimExpBlackboard() :
      aircraft_blackboard{std::make_unique<AircraftBlackboard>()},  
      camera_blackboard{std::make_unique<CameraBlackboard>()},
      input_blackboard{std::make_unique<InputBlackboard>()},
      magnum_blackboard{std::make_unique<MagnumBlackboard>()},
      sim_state_blackboard{std::make_unique<SimStateBlackboard>()} {}

    std::unique_ptr<AircraftBlackboard> aircraft_blackboard;
    std::unique_ptr<CameraBlackboard> camera_blackboard;
    std::unique_ptr<InputBlackboard> input_blackboard;
    std::unique_ptr<MagnumBlackboard> magnum_blackboard;
    std::unique_ptr<SimStateBlackboard> sim_state_blackboard;
    std::unique_ptr<TelemetryBlackboard> telemetry_blackboard;
};

inline std::shared_ptr<JSBSimExpBlackboard> make_jsbsimexp_blackboard() {
  return std::make_shared<JSBSimExpBlackboard>();
}
