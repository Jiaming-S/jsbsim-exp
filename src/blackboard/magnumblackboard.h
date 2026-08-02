#pragma once

#include <Magnum/Platform/Sdl2Application.h>
#include "Magnum/ImGuiIntegration/Context.h"

#include "blackboard.h"

#include "../types/types.h"

class MagnumBlackboard : public Blackboard {
  public:
    MagnumBlackboard() {}

    // Inputs


    // Outputs
    types::Object3D *scene_root;
    Magnum::ImGuiIntegration::Context *imgui;

    // Local

};
