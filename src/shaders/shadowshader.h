#pragma once

#include <Corrade/Utility/Resource.h>
#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/GL/Attribute.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Version.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Matrix4.h>

namespace shaders {

class ShadowShader: public Magnum::GL::AbstractShaderProgram {
  Magnum::Int _transformation_projection_matrix;
  Magnum::Int _color;
  Magnum::Int _opacity;

  public:
    // Layout
    //  - Slot 0: Position
    //  - Slot 1: Flat ground position
    typedef Magnum::GL::Attribute<0, Magnum::Vector3> Position;
    typedef Magnum::GL::Attribute<1, Magnum::Vector2> TextureCoordinates;

    explicit ShadowShader() {
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
        vert.addSource(rs.getString("src/shaders/shadowshader.vert"));
        frag.addSource(rs.getString("src/shaders/shadowshader.frag"));
      }

      CORRADE_INTERNAL_ASSERT_OUTPUT(vert.compile() && frag.compile());
      this->bindAttributeLocation(Position::Location, "position");
      this->bindAttributeLocation(TextureCoordinates::Location, "textureCoords");
      this->attachShaders({vert, frag});
      CORRADE_INTERNAL_ASSERT_OUTPUT(this->link());

      { // Cache uniform locations
        _transformation_projection_matrix = uniformLocation("uTransformationProjectionMatrix");
        _color = uniformLocation("uColor");
        _opacity = uniformLocation("uOpacity");
      }
    }

    ShadowShader& setTransformationProjectionMatrix(const Magnum::Matrix4& matrix) {
      setUniform(_transformation_projection_matrix, matrix);
      return *this;
    }

    ShadowShader& setColor(const Magnum::Color4& color) {
      setUniform(_color, color);
      return *this;
    }

    ShadowShader& setOpacity(float opacity) {
      setUniform(_opacity, opacity);
      return *this;
    }
};

}
