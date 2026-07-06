#pragma once

#include "../blackboard/jsbsimexpblackboard.h"

class Component {
  protected:
    std::shared_ptr<JSBSimExpBlackboard> blackboard;

  public:
    explicit Component(std::shared_ptr<JSBSimExpBlackboard> bb)
      : blackboard{bb} {}
    virtual ~Component() = default;
    
    virtual void init() = 0;
    virtual void handle_dispatch() = 0;
    virtual void quit() = 0;
};
