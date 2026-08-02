#pragma once

#include <Magnum/Platform/Sdl2Application.h>
#include <vector>

#include "blackboard.h"
#include "../camerahandle.h"

class CameraBlackboard : public Blackboard {
  public:
    CameraBlackboard() {}

    // Inputs
    const float camera_translation_speed_default = 4.0;
    const float camera_translation_speed_accelerated = 20.0f;
    float camera_translation_speed = 1.0;
    uint32_t active_camera_index = 0;

    // Outputs
    std::vector<CameraHandle> cameras;

    // Locals

};
