#pragma once

#include <memory>

#include "keyboardinputblackboard.h"
#include "simstateblackboard.h"

class JSBSimExpBlackboard : public Blackboard {
  public:
    JSBSimExpBlackboard() :
      input_blackboard{std::make_unique<KeyboardInputBlackboard>()},
      sim_state_blackboard{std::make_unique<SimStateBlackboard>()} {}

    std::unique_ptr<KeyboardInputBlackboard> input_blackboard;
    std::unique_ptr<SimStateBlackboard> sim_state_blackboard;
};

inline std::shared_ptr<JSBSimExpBlackboard> make_jsbsimexp_blackboard() {
  return std::make_shared<JSBSimExpBlackboard>();
}
