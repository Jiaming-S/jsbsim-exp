#pragma once

#include "component.h"

class CameraMovementComponent : public Component {
  public:
    explicit CameraMovementComponent(std::shared_ptr<JSBSimExpBlackboard> bb) 
      : Component(std::move(bb)) {}
    void init() override;
    void handle_dispatch() override;
    void quit() override;
};
