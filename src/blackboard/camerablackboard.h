#pragma once

#include <Magnum/Platform/Sdl2Application.h>
#include <vector>

#include "Magnum/Magnum.h"

#include "blackboard.h"
#include "../camerahandle.h"

class CameraBlackboard : public Blackboard {
  public:
    CameraBlackboard() {}

    // Inputs
    const float camera_translation_speed_default = 4.0;
    const float camera_translation_speed_accelerated = 20.0f;
    
    const Magnum::Deg default_camera_fov = 120.0_degf;
    const float default_camera_near_dist = 0.01f;
    const float default_camera_far_dist = INFINITY;
    
    float camera_translation_speed = 1.0f;
    size_t active_camera_index = 0;

    // Outputs
    std::vector<CameraHandle> cameras;

    // Locals

};
