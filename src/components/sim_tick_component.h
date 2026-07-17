#pragma once

#include "component.h"

class SimTickComponent : public Component {
  public:
    explicit SimTickComponent(std::shared_ptr<JSBSimExpBlackboard> bb) 
      : Component(std::move(bb)) {}
    void init() override;
    void handle_dispatch() override;
    void quit() override;
};
