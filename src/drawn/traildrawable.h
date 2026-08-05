#pragma once

#include <Corrade/Containers/ArrayViewStl.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/Math/Color.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/Shaders/FlatGL.h>

#include <deque>
#include <vector>

#include "../types/types.h"
#include "../utils/utils.h"


namespace drawn {

class TrailDrawable: public Magnum::SceneGraph::Drawable3D {
  const types::AircraftType _aircraft_type;
  const types::AircraftKeyPoints _keypoint;

  const double _ribbon_width = 2.0f;

  // Bound to corresponding AircraftHandle breadcrumbs
  std::shared_ptr<std::deque<types::AircraftTrailBreadcrumb>> _trail;
  
  Magnum::Shaders::FlatGL3D _shader;
  Magnum::Color3 _color;
  Magnum::GL::Mesh _mesh;
  Magnum::GL::Buffer _buffer;

  public:
    explicit TrailDrawable(
      types::Object3D& object,
      Magnum::SceneGraph::DrawableGroup3D& group,
      std::shared_ptr<std::deque<types::AircraftTrailBreadcrumb>> trail,
      types::AircraftType aircraft_type,
      types::AircraftKeyPoints keypoint,
      Magnum::Color3 color
    ) : Magnum::SceneGraph::Drawable3D{object, &group},
        _aircraft_type(aircraft_type),
        _keypoint(keypoint),
        _trail(trail),
        _shader{Magnum::Shaders::FlatGL3D::Configuration{}.setFlags(Magnum::Shaders::FlatGL3D::Flag::VertexColor)},
        _color(color),
        _mesh{}
    {
      _mesh.setPrimitive(Magnum::GL::MeshPrimitive::TriangleStrip);
      _mesh.addVertexBuffer(
        _buffer,
        0,
        Magnum::Shaders::FlatGL3D::Position{},
        Magnum::Shaders::FlatGL3D::Color4{}
      );
    }

    void draw(
      const Magnum::Matrix4& transformation_matrix,
      Magnum::SceneGraph::Camera3D& camera
    ) override {
      assert(_trail);
      assert(!_trail->empty());
      if (_trail->size() <= 1) return;

      struct Vertex {
        Magnum::Vector3 position;
        Magnum::Color4 color;
      };

      // Note: 2 vertices each time for flat strip: pos + dx, pos - dx
      std::vector<Vertex> vertices;

      for (size_t i = 0; i < _trail->size(); i++) {
        types::AircraftStateInfo& pos = (*_trail)[i].breadcrumb;
        
        // Magnum world-space transformation matrix for cur pos `AircraftStateInfo`
        Magnum::Matrix4 cur_transform_mat = Magnum::Matrix4::translation(utils::as_magnum_RUB(pos.north_ft, pos.east_ft, pos.down_ft))
          * Magnum::Matrix4::rotationY(-pos.yaw_rad)
          * Magnum::Matrix4::rotationX( pos.pitch_rad)
          * Magnum::Matrix4::rotationZ(-pos.roll_rad);
        
        // cur righthand delta x
        Magnum::Vector3 cur_righthand = cur_transform_mat.right() * _ribbon_width;

        // Keypoint position transformed to world-space
        Magnum::Vector3 keypoint_local = utils::to_keypoint_coords(_aircraft_type, _keypoint);
        Magnum::Vector3 keypoint = cur_transform_mat.transformPoint(keypoint_local);

        // Least transparent at high indices
        Magnum::Color4 cur_color{
          _color.r(),
          _color.g(),
          _color.b(),
          (float) i / _trail->size()
        };

        vertices.push_back(Vertex{
          keypoint + cur_righthand,
          cur_color
        });
        
        vertices.push_back(Vertex{
          keypoint - cur_righthand,
          cur_color
        });
      }

      _buffer.setData(Corrade::Containers::arrayView(vertices), Magnum::GL::BufferUsage::DynamicDraw);
      _mesh.setCount(vertices.size());

      { // Push settings
        Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::Blending);
        Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::FaceCulling);
        Magnum::GL::Renderer::setBlendFunction(
          Magnum::GL::Renderer::BlendFunction::SourceAlpha,
          Magnum::GL::Renderer::BlendFunction::OneMinusSourceAlpha
        );
      }

      _shader.setTransformationProjectionMatrix(camera.projectionMatrix() * transformation_matrix);
      _shader.draw(_mesh);
      
      { // Pop settings
        Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::FaceCulling);
        Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::Blending);
      }
    }
};

}
