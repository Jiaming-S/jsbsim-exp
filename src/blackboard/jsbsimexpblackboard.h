#pragma once

#include <memory>

#include "aircraftblackboard.h"
#include "camerablackboard.h"
#include "keyboardinputblackboard.h"
#include "magnumblackboard.h"
#include "simstateblackboard.h"

class JSBSimExpBlackboard : public Blackboard {
  public:
    JSBSimExpBlackboard() :
      aircraft_blackboard{std::make_unique<AircraftBlackboard>()},  
      camera_blackboard{std::make_unique<CameraBlackboard>()},
      keyboard_input_blackboard{std::make_unique<KeyboardInputBlackboard>()},
      magnum_blackboard{std::make_unique<MagnumBlackboard>()},
      sim_state_blackboard{std::make_unique<SimStateBlackboard>()} {}

    std::unique_ptr<AircraftBlackboard> aircraft_blackboard;
    std::unique_ptr<CameraBlackboard> camera_blackboard;
    std::unique_ptr<KeyboardInputBlackboard> keyboard_input_blackboard;
    std::unique_ptr<MagnumBlackboard> magnum_blackboard;
    std::unique_ptr<SimStateBlackboard> sim_state_blackboard;
};

inline std::shared_ptr<JSBSimExpBlackboard> make_jsbsimexp_blackboard() {
  return std::make_shared<JSBSimExpBlackboard>();
}
