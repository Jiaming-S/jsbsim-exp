#pragma once

#include "component.h"

class CameraControlComponent : public Component {
  public:
    explicit CameraControlComponent(std::shared_ptr<JSBSimExpBlackboard> bb) 
      : Component(std::move(bb)) {}
    void init() override;
    void handle_dispatch() override;
    void quit() override;
};
