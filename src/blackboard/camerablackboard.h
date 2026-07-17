#pragma once

#include <Magnum/Platform/Sdl2Application.h>
#include <vector>

#include "blackboard.h"
#include "../camerahandle.h"

class CameraBlackboard : public Blackboard {
  public:
    CameraBlackboard() {}

    // Inputs
    uint32_t active_camera_index = 0;

    // Outputs
    std::vector<CameraHandle> cameras;

    // Locals

};
