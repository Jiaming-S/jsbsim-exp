#pragma once

#include <Magnum/Platform/Sdl2Application.h>

#include "component.h"

class KeyboardInputComponent : public Component {
  public:
    explicit KeyboardInputComponent(std::shared_ptr<JSBSimExpBlackboard> bb, Sdl2Application *app) 
      : Component(std::move(bb), app) {}
    void init() override;
    void handle_dispatch() override;
    void quit() override;
};
