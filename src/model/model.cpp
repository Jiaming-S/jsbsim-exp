#include "model.h"

namespace model {


void _traverse_scene_graph(
  Magnum::Trade::AbstractImporter& importer, 
  Magnum::UnsignedInt node_id, 
  const Magnum::Matrix4& parent_transform, 
  std::shared_ptr<ModelMultipartTextured>& target_model
) {
  Corrade::Containers::Pointer<Magnum::Trade::ObjectData3D> object_data = importer.object3D(node_id);
  if (!object_data) return;

  Magnum::Matrix4 absolute_transform = parent_transform * object_data->transformation();

  if (object_data->instanceType() == Magnum::Trade::ObjectInstanceType3D::Mesh) {
    Magnum::UnsignedInt mesh_idx = object_data->instance();
    size_t texture_idx = std::numeric_limits<size_t>::max();

    const auto& mesh_object = static_cast<const Magnum::Trade::MeshObjectData3D&>(*object_data);
    Magnum::Int mat_id = mesh_object.material();

    if (mat_id != -1) {
      Corrade::Containers::Optional<Magnum::Trade::MaterialData> material_data = importer.material(mat_id);
      if (material_data && material_data->hasAttribute(Magnum::Trade::MaterialAttribute::BaseColorTexture)) {
        texture_idx = material_data->attribute<Magnum::UnsignedInt>(Magnum::Trade::MaterialAttribute::BaseColorTexture);
      }
    }

    target_model->_components.push_back(ModelComponent{
      static_cast<size_t>(mesh_idx),
      texture_idx,
      absolute_transform
    });
  }

  for (Magnum::UnsignedInt child_id : object_data->children()) {
    _traverse_scene_graph(importer, child_id, absolute_transform, target_model);
  }
}

void ModelRepository::ingest_asset_glb(
  Corrade::Utility::Resource& rs,
  std::string asset_name,
  std::string asset_filepath
) {
  Corrade::PluginManager::Manager<Magnum::Trade::AbstractImporter> manager;
  Corrade::Containers::Pointer<Magnum::Trade::AbstractImporter> importer = manager.loadAndInstantiate("TinyGltfImporter");

  // Read asset from Corrade resource manager
  importer->openData(rs.getRaw(asset_filepath));

  auto target_model = std::make_shared<ModelMultipartTextured>();
  target_model->_textures.reserve(importer->textureCount());

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
      for (Magnum::UnsignedInt root_id : scene_data->children3D()) {
        _traverse_scene_graph(*importer, root_id, root_correction, target_model);
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
