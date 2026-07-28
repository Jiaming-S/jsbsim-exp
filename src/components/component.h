#pragma once

#include "../blackboard/jsbsimexpblackboard.h"
#include "Magnum/Platform/Sdl2Application.h"

class Component {
  protected:
    std::shared_ptr<JSBSimExpBlackboard> blackboard;
    Sdl2Application *app;

  public:
    explicit Component(
      std::shared_ptr<JSBSimExpBlackboard> bb,
      Sdl2Application *app
    ) : blackboard{bb}, app{app} {}
    virtual ~Component() = default;
    
    virtual void init() = 0;
    virtual void handle_dispatch() = 0;
    virtual void quit() = 0;
};
