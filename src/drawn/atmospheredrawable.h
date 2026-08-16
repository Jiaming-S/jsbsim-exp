#pragma once

#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Primitives/Square.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/Trade/MeshData.h>

#include "../shaders/skyshader.h"
#include "../types/types.h"


namespace drawn {

class AtmosphereDrawable: public Magnum::SceneGraph::Drawable3D {
  shaders::SkyShader& _shader;
  Magnum::GL::Mesh _mesh;
  
  public:
    explicit AtmosphereDrawable(
      types::Object3D& root,
      shaders::SkyShader& shader,
      Magnum::SceneGraph::DrawableGroup3D& group
    ) : Magnum::SceneGraph::Drawable3D{root, &group},
        _shader(shader),
        _mesh(Magnum::MeshTools::compile(Magnum::Primitives::squareSolid())) {}

  private:
    void draw(
      const Magnum::Matrix4& transformationMatrix,
      Magnum::SceneGraph::Camera3D& camera
    ) override {
      { // Push Environment required settings
        Magnum::GL::Renderer::setDepthMask(GL_FALSE);
        Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::DepthTest);
      }
      
      Magnum::Matrix4 camera_from_root_pos = camera.object().absoluteTransformationMatrix();
      _shader.setProjectionMatrix(camera.projectionMatrix())
        .setCameraWorldPos(camera_from_root_pos.translation())
        .setViewMatrix(camera_from_root_pos.inverted())
        .setSunDirection({0.3f, 0.5f, -0.6f});
      _shader.draw(_mesh);

      { // Pop Environment required settings
        Magnum::GL::Renderer::setDepthMask(GL_TRUE);
        Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::DepthTest);
      }
    }
};

};
