#pragma once

#include <memory>

#include "aircraftblackboard.h"
#include "camerablackboard.h"
#include "keyboardinputblackboard.h"
#include "simstateblackboard.h"

class JSBSimExpBlackboard : public Blackboard {
  public:
    JSBSimExpBlackboard() :
      input_blackboard{std::make_unique<KeyboardInputBlackboard>()},
      sim_state_blackboard{std::make_unique<SimStateBlackboard>()},
      aircraft_blackboard_vec{std::make_unique<AircraftBlackboard>()},
      camera_blackboard_vec{std::make_unique<CameraBlackboard>()} {}

    std::unique_ptr<KeyboardInputBlackboard> input_blackboard;
    std::unique_ptr<SimStateBlackboard> sim_state_blackboard;
    std::unique_ptr<AircraftBlackboard> aircraft_blackboard_vec;
    std::unique_ptr<CameraBlackboard> camera_blackboard_vec;
};

inline std::shared_ptr<JSBSimExpBlackboard> make_jsbsimexp_blackboard() {
  return std::make_shared<JSBSimExpBlackboard>();
}
