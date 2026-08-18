#pragma once

#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/SceneGraph/Drawable.h>
#include "Magnum/ImGuiIntegration/Context.h"

#include "blackboard.h"

#include "../types/types.h"
#include "../model/model.h"
#include "../shaders/floorshader.h"
#include "../shaders/skyshader.h"
#include "../shaders/shadowshader.h"

class MagnumBlackboard : public Blackboard {
  public:
    MagnumBlackboard() {}

    // Inputs
    types::Object3D *scene_root;
    Magnum::ImGuiIntegration::Context *imgui_ctx;

    Magnum::SceneGraph::DrawableGroup3D *background_drawables;
    Magnum::SceneGraph::DrawableGroup3D *drawables;

    model::GLBModelRepository *model_repo;

    Magnum::Shaders::PhongGL *phong_shader;
    shaders::FloorShader *floor_shader;
    shaders::SkyShader *sky_shader;
    shaders::ShadowShader *shadow_shader;


    // Outputs

    // Local

};
