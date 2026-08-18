#include "camera_draw_component.h"

void CameraDrawComponent::init() {}

void CameraDrawComponent::quit() {}

void CameraDrawComponent::handle_dispatch() {
  Magnum::SceneGraph::DrawableGroup3D *background_drawables = blackboard->magnum_blackboard->background_drawables;
  Magnum::SceneGraph::DrawableGroup3D *drawables = blackboard->magnum_blackboard->drawables;

  const size_t active_camera_index = blackboard->camera_blackboard->active_camera_index;
  const CameraHandle camera_handle = blackboard->camera_blackboard->cameras[active_camera_index];
  
  Magnum::SceneGraph::Camera3D *active_camera = camera_handle._camera;

  active_camera->draw(*background_drawables);
  active_camera->draw(*drawables);
}
