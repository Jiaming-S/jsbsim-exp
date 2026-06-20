#pragma once

#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Primitives/Square.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/Trade/MeshData.h>

#include "../shaders/shadowshader.h"
#include "../types/types.h"

namespace drawn {

class ShadowDrawable: public Magnum::SceneGraph::Drawable3D {
  shaders::ShadowShader& _shader;
  Magnum::GL::Mesh _mesh;
  float _radius;

  public:
    explicit ShadowDrawable(
      types::Object3D& aircraft_object,
      shaders::ShadowShader& shader,
      Magnum::SceneGraph::DrawableGroup3D& group,
      float radius = 15.0f
    ) : Magnum::SceneGraph::Drawable3D{aircraft_object, &group},
        _shader(shader),
        _mesh(Magnum::MeshTools::compile(Magnum::Primitives::squareSolid(Magnum::Primitives::SquareFlag::TextureCoordinates))),
        _radius(radius) {}

  private:
    void draw(
      const Magnum::Matrix4& transformationMatrix,
      Magnum::SceneGraph::Camera3D& camera
    ) override {
      // Get aircraft' world position
      Magnum::Matrix4 aircraft_world = object().absoluteTransformationMatrix();
      Magnum::Vector3 translation_world = aircraft_world.translation();

      // Calculate aircraft altitude
      float altitude = translation_world.y();

      // Calculate opacity based on altitude
      // Fade out to 0.0 when altitude reaches 400.0 meters
      float opacity = 1.0f - Magnum::Math::clamp(altitude / 400.0f, 0.0f, 1.0f);
      if (opacity <= 0.001f) return;

      // Draw shadow at Y = 0.01f
      Magnum::Matrix4 shadow_world = Magnum::Matrix4::translation({translation_world.x(), 0.01f, translation_world.z()})
        * Magnum::Matrix4::rotationX(Magnum::Deg(-90.0f))
        * Magnum::Matrix4::scaling(Magnum::Vector3{_radius});

      // Transform to camera space
      Magnum::Matrix4 view_matrix = camera.object().absoluteTransformationMatrix().inverted();
      Magnum::Matrix4 shadow_transformation = view_matrix * shadow_world;
      Magnum::Matrix4 shadow_transformation_projection = camera.projectionMatrix() * shadow_transformation;

      // Draw using shader
      _shader.setTransformationProjectionMatrix(shadow_transformation_projection)
        .setColor({0.05f, 0.05f, 0.05f, 0.6f}) // dark grey shadow, 60% opacity at ground level
        .setOpacity(opacity);

      { // Push settings
        Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::Blending);
        Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::FaceCulling);
        Magnum::GL::Renderer::setDepthMask(GL_FALSE);
        Magnum::GL::Renderer::setBlendFunction(
          Magnum::GL::Renderer::BlendFunction::SourceAlpha,
          Magnum::GL::Renderer::BlendFunction::OneMinusSourceAlpha
        );
      }

      _mesh.draw(_shader);

      { // Pop settings
        Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::FaceCulling);
        Magnum::GL::Renderer::setDepthMask(GL_TRUE);
        Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::Blending);
      }
    }
};

}
