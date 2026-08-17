#include "model.h"
#include <Magnum/PixelFormat.h>
#include <Magnum/Math/Color.h>
#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/Pair.h>
#include <unordered_map>
#include <string>
#include <cctype>

namespace model {


void _traverse_scene_graph(
  Magnum::Trade::AbstractImporter& importer, 
  const Magnum::Trade::SceneData& scene_data,
  Magnum::UnsignedLong node_id, 
  const Magnum::Matrix4& parent_transform, 
  std::shared_ptr<ModelMultipartTextured>& target_model,
  size_t fallback_texture_idx,
  std::unordered_map<Magnum::Int, size_t>& material_texture_cache
) {
  auto opt_transform = scene_data.transformation3DFor(node_id);
  Magnum::Matrix4 local_transform = opt_transform ? *opt_transform : Magnum::Matrix4{};
  Magnum::Matrix4 absolute_transform = parent_transform * local_transform;

  auto meshes_materials = scene_data.meshesMaterialsFor(node_id);
  for (const auto& mesh_material : meshes_materials) {
    Magnum::UnsignedInt mesh_idx = mesh_material.first();
    Magnum::Int mat_id = mesh_material.second();

    size_t texture_idx = fallback_texture_idx;

    if (mat_id != -1) {
      auto cached_texture = material_texture_cache.find(mat_id);
      if (cached_texture != material_texture_cache.end()) {
        texture_idx = cached_texture->second;
      } else {
        Corrade::Containers::Optional<Magnum::Trade::MaterialData> material_data = importer.material(mat_id);
        if (material_data) {
          if (material_data->hasAttribute(Magnum::Trade::MaterialAttribute::BaseColorTexture)) {
            texture_idx = material_data->attribute<Magnum::UnsignedInt>(Magnum::Trade::MaterialAttribute::BaseColorTexture);
            material_texture_cache[mat_id] = texture_idx;
          } else {
            // Get base color or fallback
            Magnum::Color4 base_color{1.0f, 1.0f, 1.0f, 1.0f};
            if (material_data->hasAttribute(Magnum::Trade::MaterialAttribute::BaseColor)) {
              base_color = material_data->attribute<Magnum::Color4>(Magnum::Trade::MaterialAttribute::BaseColor);
            }

            // Check if material name is canopy or glass to make it gold reflective
            std::string mat_name = importer.materialName(mat_id);
            for (auto& c : mat_name) c = std::tolower(c);

            if (mat_name.find("canopy") != std::string::npos || mat_name.find("glass") != std::string::npos) {
              // Set to a beautiful reflective golden color
              base_color = Magnum::Color4{0.92f, 0.78f, 0.32f, 1.0f};
            }

            // Create 1x1 color texture
            Magnum::Vector4ub color_ub{
                static_cast<Magnum::UnsignedByte>(base_color.r() * 255.0f),
                static_cast<Magnum::UnsignedByte>(base_color.g() * 255.0f),
                static_cast<Magnum::UnsignedByte>(base_color.b() * 255.0f),
                static_cast<Magnum::UnsignedByte>(base_color.a() * 255.0f)
            };

            Magnum::ImageView2D image{
                Magnum::PixelFormat::RGBA8Unorm,
                {1, 1},
                Corrade::Containers::ArrayView<const void>{&color_ub, 4}
            };

            Magnum::GL::Texture2D texture;
            texture.setMagnificationFilter(Magnum::GL::SamplerFilter::Nearest)
                   .setMinificationFilter(Magnum::GL::SamplerFilter::Nearest)
                   .setStorage(1, Magnum::GL::TextureFormat::RGBA8, {1, 1})
                   .setSubImage(0, {}, image);

            target_model->_textures.push_back(std::move(texture));
            texture_idx = target_model->_textures.size() - 1;
            material_texture_cache[mat_id] = texture_idx;
          }
        }
      }
    }

    target_model->_components.push_back(ModelComponent{
      static_cast<size_t>(mesh_idx),
      texture_idx,
      absolute_transform
    });
  }

  for (Magnum::UnsignedLong child_id : scene_data.childrenFor(node_id)) {
    _traverse_scene_graph(importer, scene_data, child_id, absolute_transform, target_model, fallback_texture_idx, material_texture_cache);
  }
}

void ModelRepository::ingest_asset_glb(
  std::string asset_name,
  std::string asset_filepath
) {
  Corrade::PluginManager::Manager<Magnum::Trade::AbstractImporter> manager;
  Corrade::Containers::Pointer<Magnum::Trade::AbstractImporter> importer = manager.loadAndInstantiate("TinyGltfImporter");

  // Read asset from Corrade resource manager
  Corrade::Utility::Resource rs{"assets"};
  importer->openData(rs.getRaw(asset_filepath));

  auto target_model = std::make_shared<ModelMultipartTextured>();
  target_model->_textures.reserve(importer->textureCount() + 8); // reserve extra space for fallback & dynamic textures

  // 1A. Load all Textures
  for (Magnum::UnsignedInt i = 0; i < importer->textureCount(); ++i) {
    Corrade::Containers::Optional<Magnum::Trade::TextureData> texture_data = importer->texture(i);
    Magnum::GL::Texture2D texture; 

    if (texture_data && texture_data->type() == Magnum::Trade::TextureType::Texture2D) {
      Corrade::Containers::Optional<Magnum::Trade::ImageData2D> image_data = importer->image2D(texture_data->image());
      if (image_data) {
        texture.setMagnificationFilter(texture_data->magnificationFilter())
          .setMinificationFilter(texture_data->minificationFilter(), texture_data->mipmapFilter())
          .setWrapping(texture_data->wrapping().xy())
          .setStorage(1, Magnum::GL::textureFormat(image_data->format()), image_data->size())
          .setSubImage(0, Magnum::Vector2i{}, *image_data);
      }
    }
    target_model->_textures.push_back(std::move(texture));
  }

  // Create default fallback white texture
  size_t fallback_texture_idx = target_model->_textures.size();
  {
    Magnum::Vector4ub color_ub{255, 255, 255, 255};
    Magnum::ImageView2D image{
        Magnum::PixelFormat::RGBA8Unorm,
        {1, 1},
        Corrade::Containers::ArrayView<const void>{&color_ub, 4}
    };
    Magnum::GL::Texture2D texture;
    texture.setMagnificationFilter(Magnum::GL::SamplerFilter::Nearest)
           .setMinificationFilter(Magnum::GL::SamplerFilter::Nearest)
           .setStorage(1, Magnum::GL::TextureFormat::RGBA8, {1, 1})
           .setSubImage(0, {}, image);
    target_model->_textures.push_back(std::move(texture));
  }

  // 1B. Load all Meshes
  target_model->_meshes.reserve(importer->meshCount());
  for (Magnum::UnsignedInt i = 0; i < importer->meshCount(); ++i) {
    Corrade::Containers::Optional<Magnum::Trade::MeshData> mesh_data = importer->mesh(i);
    if (mesh_data) {
      target_model->_meshes.push_back(Magnum::MeshTools::compile(*mesh_data));
    } else {
      target_model->_meshes.push_back(Magnum::GL::Mesh{}); 
    }
  }

  // 2. Build the Blueprints via Recursive Helper
  if (importer->defaultScene() != -1) {
    Corrade::Containers::Optional<Magnum::Trade::SceneData> scene_data = importer->scene(importer->defaultScene());
    
    if (scene_data) {
      Magnum::Matrix4 root_correction = Magnum::Matrix4::rotationY(Magnum::Deg(90.0f));
      std::unordered_map<Magnum::Int, size_t> material_texture_cache;
      for (Magnum::UnsignedLong root_id : scene_data->childrenFor(-1)) {
        _traverse_scene_graph(*importer, *scene_data, root_id, root_correction, target_model, fallback_texture_idx, material_texture_cache);
      }
    }
  }

  // 3. Store in Repository
  _all_models[asset_name] = std::move(target_model);
}

std::shared_ptr<ModelMultipartTextured> ModelRepository::get_aircraft_model(types::AircraftType type) {
  return _all_models[utils::to_type_string(type)];
}

};
