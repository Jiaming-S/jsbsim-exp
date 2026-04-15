#include "utils.h"

void _construct_tmp_jsbsim_dir(
  Corrade::Utility::Resource& _rs,
  std::string aircraft_type
) {
  std::string tmp_dir = (*Corrade::Utility::Path::temporaryDirectory()) + "/jsbsim-flightmodels/";
  Magnum::Utility::Path::make(tmp_dir);
  Magnum::Utility::Path::make(tmp_dir + "aircraft/");
  Magnum::Utility::Path::make(tmp_dir + "systems/");
  Magnum::Utility::Path::make(tmp_dir + "engine/");

  Magnum::Utility::Path::make(tmp_dir + "aircraft/" + aircraft_type);
  if (aircraft_type == "f16") {
    Magnum::Utility::Path::write(tmp_dir + "aircraft/f16/f16.xml",     _rs.getString("assets/f16/flightmodel/f16.xml"));
    Magnum::Utility::Path::write(tmp_dir + "aircraft/f16/reset00.xml", _rs.getString("assets/f16/flightmodel/reset00.xml"));

    Magnum::Utility::Path::write(tmp_dir + "systems/hook.xml", _rs.getString("assets/f16/flightmodel/hook.xml"));
    Magnum::Utility::Path::write(tmp_dir + "systems/pushback.xml", _rs.getString("assets/f16/flightmodel/pushback.xml"));

    Magnum::Utility::Path::write(tmp_dir + "engine/F100-PW-229.xml", _rs.getString("assets/f16/flightmodel/F100-PW-229.xml"));
    Magnum::Utility::Path::write(tmp_dir + "engine/direct.xml", _rs.getString("assets/f16/flightmodel/direct.xml"));
  }
}

void load_aircraft(
  std::unique_ptr<JSBSim::FGFDMExec>& aircraft_fdmexec,
  std::string aircraft_type_dir,
  std::string aircraft_ic_file,
  bool quiet
) {
  // Store original debug level
  short orig_debug_level = JSBSim::FGJSBBase::debug_lvl;
  if (quiet) JSBSim::FGJSBBase::debug_lvl = 0; 

  // Load from tmp dir
  std::string tmp_dir = (*Corrade::Utility::Path::temporaryDirectory()) + "/jsbsim-flightmodels/";
  
  // Import aircraft (default "f16")
  aircraft_fdmexec->SetAircraftPath(SGPath(tmp_dir + "/aircraft"));
  aircraft_fdmexec->SetEnginePath(SGPath(tmp_dir + "/engine"));
  aircraft_fdmexec->SetSystemsPath(SGPath(tmp_dir + "/systems"));
  aircraft_fdmexec->LoadModel(aircraft_type_dir);

  // Import aircraft initial conditions (default "reset00.xml")
  std::shared_ptr<JSBSim::FGInitialCondition> aircraft_ic = aircraft_fdmexec->GetIC();
  aircraft_ic->InitializeIC();
  aircraft_ic->Load(SGPath(aircraft_ic_file));

  // Load initial conditions
  aircraft_fdmexec->RunIC();

  // Restore original debug level
  JSBSim::FGJSBBase::debug_lvl = orig_debug_level;
}

// Recursively flatten GLTF
void flatten_gltf_nodes(
  Magnum::Trade::AbstractImporter& importer, 
  Magnum::UnsignedInt node_id, 
  const Magnum::Matrix4& parent_transform, 
  std::vector<types::ModelPart>& parts
) {
                        
  Corrade::Containers::Pointer<Magnum::Trade::ObjectData3D> object = importer.object3D(node_id);
  if (!object) return;

  Magnum::Matrix4 absolute_transform = parent_transform * object->transformation();
  if (object->instanceType() == Magnum::Trade::ObjectInstanceType3D::Mesh) {
    Corrade::Containers::Optional<Magnum::Trade::MeshData> meshData = importer.mesh(object->instance());
    if (meshData) parts.push_back({Magnum::MeshTools::compile(*meshData), absolute_transform});
  }

  for (Magnum::UnsignedInt child_id : object->children()) {
    flatten_gltf_nodes(importer, child_id, absolute_transform, parts);
  }
}

void load_meshes(
  Corrade::Utility::Resource& _rs,
  std::unordered_map<std::string, std::vector<types::ModelPart>>& _meshes,
  const std::vector<std::pair<std::string, std::string>>& to_import
) {
  Corrade::PluginManager::Manager<Magnum::Trade::AbstractImporter> manager;
  Corrade::Containers::Pointer<Magnum::Trade::AbstractImporter> importer = manager.loadAndInstantiate("TinyGltfImporter");
  assert(importer);

  for (const auto& item : to_import) {
    const std::string& model_name = item.first;
    const std::string& model_filepath = item.second;

    if (!importer->openData(_rs.getRaw(model_filepath))) continue;

    std::vector<types::ModelPart> model_parts;

    // Start flattening from default scene root
    if (importer->defaultScene() != -1) {
      Corrade::Containers::Optional<Magnum::Trade::SceneData> scene = importer->scene(importer->defaultScene());
      if (scene) {
        for(Magnum::UnsignedInt root_id : scene->children3D()) {
          flatten_gltf_nodes(*importer, root_id, Magnum::Matrix4{}, model_parts);
        }
      }
    }
    else {
      // If there is no scene hierarchy
      for(Magnum::UnsignedInt i = 0; i < importer->meshCount(); ++i) {
        Corrade::Containers::Optional<Magnum::Trade::MeshData> meshData = importer->mesh(i);
        if (meshData) model_parts.push_back({Magnum::MeshTools::compile(*meshData), Magnum::Matrix4{}});
      }
    }

    _meshes[model_name] = std::move(model_parts);
  }
}