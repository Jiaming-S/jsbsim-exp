#pragma once

#include "component.h"

#include "../gui/gui.h"

class HandOfGodControlComponent : public Component {
  public:
    explicit HandOfGodControlComponent(std::shared_ptr<JSBSimExpBlackboard> bb, Sdl2Application *app) 
      : Component(std::move(bb), app) {}
    void init() override;
    void handle_dispatch() override;
    void quit() override;
};
