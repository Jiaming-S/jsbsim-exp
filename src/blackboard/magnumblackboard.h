#pragma once

#include <Magnum/Platform/Sdl2Application.h>

#include "blackboard.h"

#include "../types/types.h"

class MagnumBlackboard : public Blackboard {
  public:
    MagnumBlackboard() {}

    // Inputs


    // Outputs
    types::Object3D *scene_root;
    types::Object3D *environment_root;
    
    std::vector<types::Object3D*> all_objects;

    // Local

};
