#pragma once

#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/SceneGraph/Drawable.h>
#include "Magnum/ImGuiIntegration/Context.h"

#include "blackboard.h"

#include "../types/types.h"

class MagnumBlackboard : public Blackboard {
  public:
    MagnumBlackboard() {}

    // Inputs


    // Outputs
    types::Object3D *scene_root;
    Magnum::ImGuiIntegration::Context *imgui_ctx;

    // TODO: make these actually used, needs a CameraDrawComponent
    Magnum::SceneGraph::DrawableGroup3D _background_drawables;
    Magnum::SceneGraph::DrawableGroup3D _drawables;

    // Local

};
