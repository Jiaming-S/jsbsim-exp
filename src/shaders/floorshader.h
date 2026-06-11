#pragma once

#include <Corrade/Utility/Resource.h>
#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/GL/Attribute.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/Version.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/Math/Vector3.h>

namespace shaders {

class FloorShader: public Magnum::GL::AbstractShaderProgram {
  Magnum::Int _projection_matrix;
  Magnum::Int _camera_pos;
  Magnum::Int _view_matrix;
  Magnum::Int _grid_spacing;

  // Layout
  //  - Slot 0: Position
  typedef Magnum::GL::Attribute<0, Magnum::Vector3> Position;

  public:
    explicit FloorShader() {
      Magnum::GL::Shader vert{
        Magnum::GL::Version::GL410,
        Magnum::GL::Shader::Type::Vertex
      };

      Magnum::GL::Shader frag{
        Magnum::GL::Version::GL410,
        Magnum::GL::Shader::Type::Fragment
      };

      { // Load shader files
        Corrade::Utility::Resource rs{"assets"};
        vert.addSource(rs.getString("src/shaders/floorshader.vert"));
        frag.addSource(rs.getString("src/shaders/floorshader.frag"));
      }

      CORRADE_INTERNAL_ASSERT_OUTPUT(vert.compile() && frag.compile());
      this->bindAttributeLocation(Position::Location, "position");
      this->attachShaders({vert, frag});
      CORRADE_INTERNAL_ASSERT_OUTPUT(this->link());

      { // Cache uniform locations
        _projection_matrix = uniformLocation("uProjectionMatrix");
        _view_matrix = uniformLocation("uViewMatrix");
        _camera_pos = uniformLocation("uCameraWorldPos");
        _grid_spacing = uniformLocation("uGridSpacing");
      }
    }

    FloorShader& setProjectionMatrix(const Magnum::Matrix4& matrix) {
      setUniform(_projection_matrix, matrix);
      return *this;
    }
    FloorShader& setCameraWorldPos(const Magnum::Vector3& pos) {
      setUniform(_camera_pos, pos);
      return *this;
    }
    FloorShader& setViewMatrix(const Magnum::Matrix4& matrix) {
      setUniform(_view_matrix, matrix);
      return *this;
    }
    FloorShader& setGridSpacing(float spacing) {
      setUniform(_grid_spacing, spacing);
      return *this;
    }
};

};


