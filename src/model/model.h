#pragma once

#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/Texture.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/ImageView.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/Math/Vector2.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Trade/AbstractImporter.h>
#include <Magnum/Trade/SceneData.h>
#include <Magnum/Trade/MeshData.h>
#include <Magnum/Trade/MeshObjectData3D.h>
#include <Magnum/Trade/TextureData.h>
#include <Magnum/Trade/ImageData.h>
#include <Magnum/Trade/MaterialData.h>
#include <Corrade/PluginManager/Manager.h>

#include <functional>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "../types/types.h"
#include "../utils/utils.h"

using namespace Magnum::Math::Literals;


namespace model {

struct ModelComponent {
  size_t mesh_idx;
  size_t texture_idx;
  Magnum::Matrix4 local_transformation;
};

struct ModelMultipartTextured {
  std::vector<Magnum::GL::Mesh> _meshes;
  std::vector<Magnum::GL::Texture2D> _textures;
  std::vector<model::ModelComponent> _components;
};

class ModelRepository {
  private:
    std::unordered_map<std::string, std::shared_ptr<ModelMultipartTextured>> _all_models;

  public:
    void ingest_asset_glb(std::string asset_name, std::string asset_filepath);
    std::shared_ptr<ModelMultipartTextured> get_aircraft_model(types::AircraftType type = types::AircraftType::F16);
};

};
