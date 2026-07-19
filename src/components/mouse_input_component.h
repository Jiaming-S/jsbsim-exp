#pragma once

#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Magnum.h>

#include "component.h"

class MouseInputComponent : public Component {
  public:
    explicit MouseInputComponent(std::shared_ptr<JSBSimExpBlackboard> bb) 
      : Component(std::move(bb)) {}
    void init() override;
    void handle_dispatch() override;
    void quit() override;
};
