#pragma once

#include "component.h"

#include "../gui/gui.h"

class GuiComponent : public Component {
  public:
    explicit GuiComponent(std::shared_ptr<JSBSimExpBlackboard> bb) 
      : Component(std::move(bb)) {}
    void init() override;
    void handle_dispatch() override;
    void quit() override;
};
