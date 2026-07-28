#pragma once

#include "component.h"

#include "../gui/gui.h"

class GuiComponent : public Component {
  public:
    explicit GuiComponent(std::shared_ptr<JSBSimExpBlackboard> bb, Sdl2Application *app) 
      : Component(std::move(bb), app) {}
    void init() override;
    void handle_dispatch() override;
    void quit() override;
};
