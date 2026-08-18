#pragma once

#include <memory>

#include "aircraftblackboard.h"
#include "camerablackboard.h"
#include "handofgodblackboard.h"
#include "inputblackboard.h"
#include "magnumblackboard.h"
#include "networkblackboard.h"
#include "simstateblackboard.h"
#include "telemetryblackboard.h"

class JSBSimExpBlackboard : public Blackboard {
  public:
    std::unique_ptr<AircraftBlackboard> aircraft_blackboard;
    std::unique_ptr<CameraBlackboard> camera_blackboard;
    std::unique_ptr<HandOfGodBlackboard> hand_of_god_blackboard;
    std::unique_ptr<InputBlackboard> input_blackboard;
    std::unique_ptr<MagnumBlackboard> magnum_blackboard;
    std::unique_ptr<NetworkBlackboard> network_blackboard;
    std::unique_ptr<SimStateBlackboard> sim_state_blackboard;
    std::unique_ptr<TelemetryBlackboard> telemetry_blackboard;

    JSBSimExpBlackboard() :
      aircraft_blackboard{std::make_unique<AircraftBlackboard>()},  
      camera_blackboard{std::make_unique<CameraBlackboard>()},
      hand_of_god_blackboard{std::make_unique<HandOfGodBlackboard>()},
      input_blackboard{std::make_unique<InputBlackboard>()},
      magnum_blackboard{std::make_unique<MagnumBlackboard>()},
      network_blackboard{std::make_unique<NetworkBlackboard>()},
      sim_state_blackboard{std::make_unique<SimStateBlackboard>()},
      telemetry_blackboard{std::make_unique<TelemetryBlackboard>()} {}
};

inline std::shared_ptr<JSBSimExpBlackboard> make_jsbsimexp_blackboard() {
  return std::make_shared<JSBSimExpBlackboard>();
}
