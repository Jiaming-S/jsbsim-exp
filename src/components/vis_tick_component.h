#pragma once

#include "component.h"

class VisTickComponent : public Component {
  public:
    explicit VisTickComponent(std::shared_ptr<JSBSimExpBlackboard> bb) 
      : Component(std::move(bb)) {}
    void init() override;
    void handle_dispatch() override;
    void quit() override;
};
