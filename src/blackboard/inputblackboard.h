#pragma once

#include <Magnum/Platform/Sdl2Application.h>
#include <unordered_set>

#include "blackboard.h"

class InputBlackboard : public Blackboard {
  public:
    InputBlackboard() {}

    // Inputs
    std::unordered_set<Magnum::Platform::Sdl2Application::Key> keys_down;
    bool mouse_held = false;
    float mouse_sensitivity = 0.1f;
    Magnum::Vector2 mouse_position{0.0f, 0.0f};

    // Outputs
    Magnum::Float commanded_aircraft_throttle{0.0f};
    Magnum::Float commanded_aircraft_braking{0.0f};
    Magnum::Float commanded_aircraft_roll{0.0f};
    Magnum::Float commanded_aircraft_yaw{0.0f};
    Magnum::Float commanded_aircraft_pitch{0.0f};
    
    Magnum::Vector3 commanded_camera_translation{0.0f, 0.0f, 0.0f};
    Magnum::Float commanded_camera_roll{0.0f};
    Magnum::Float commanded_camera_yaw{0.0f};
    Magnum::Float commanded_camera_pitch{0.0f};

    // Local    
    std::unordered_set<Magnum::Platform::Sdl2Application::Key> prev_keys_down;
    Magnum::Vector2 prev_mouse_position{0.0f, 0.0f};
    Magnum::Vector2 mouse_delta{0.0f, 0.0f};
};
