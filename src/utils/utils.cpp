#include "utils.h"

#define PUSH_JSBSIM_DEBUG_LEVEL \
  short orig_debug_level = JSBSim::FGJSBBase::debug_lvl; \
  if (quiet) JSBSim::FGJSBBase::debug_lvl = 0; 
#define POP_JSBSIM_DEBUG_LEVEL \
  JSBSim::FGJSBBase::debug_lvl = orig_debug_level;

namespace utils {

void _construct_tmp_jsbsim_dir(
  Corrade::Utility::Resource& _rs,
  types::AircraftType aircraft_type
) {
  std::string tmp_dir = (*Corrade::Utility::Path::temporaryDirectory()) + "/jsbsim-flightmodels/";
  Magnum::Utility::Path::make(tmp_dir);
  Magnum::Utility::Path::make(tmp_dir + "aircraft/");
  Magnum::Utility::Path::make(tmp_dir + "systems/");
  Magnum::Utility::Path::make(tmp_dir + "engine/");

  switch (aircraft_type) {
    case types::AircraftType::F16:
      Magnum::Utility::Path::make(tmp_dir + "aircraft/f16/");

      // Fightmodel
      Magnum::Utility::Path::write(tmp_dir + "aircraft/f16/f16.xml",     _rs.getString("assets/f16/flightmodel/f16.xml"));
      Magnum::Utility::Path::write(tmp_dir + "aircraft/f16/reset00.xml", _rs.getString("assets/f16/flightmodel/reset00.xml"));
      // Systems
      Magnum::Utility::Path::write(tmp_dir + "systems/hook.xml", _rs.getString("assets/f16/flightmodel/hook.xml"));
      Magnum::Utility::Path::write(tmp_dir + "systems/pushback.xml", _rs.getString("assets/f16/flightmodel/pushback.xml"));
      // Engine
      Magnum::Utility::Path::write(tmp_dir + "engine/F100-PW-229.xml", _rs.getString("assets/f16/flightmodel/F100-PW-229.xml"));
      Magnum::Utility::Path::write(tmp_dir + "engine/direct.xml", _rs.getString("assets/f16/flightmodel/direct.xml"));
      break;
    default: break;
  }
}

std::unique_ptr<JSBSim::FGFDMExec> load_aircraft(
  types::AircraftType aircraft_type,
  std::string aircraft_type_string,
  bool quiet
) {
  PUSH_JSBSIM_DEBUG_LEVEL
  std::unique_ptr<JSBSim::FGFDMExec> aircraft_fdmexec = std::make_unique<JSBSim::FGFDMExec>();

  // Load and attach to tmp dir
  std::string tmp_dir = (*Corrade::Utility::Path::temporaryDirectory()) + "/jsbsim-flightmodels/";
  aircraft_fdmexec->SetAircraftPath(SGPath(tmp_dir + "aircraft/"));
  aircraft_fdmexec->SetEnginePath(SGPath(tmp_dir + "engine/"));
  aircraft_fdmexec->SetSystemsPath(SGPath(tmp_dir + "systems/"));

  // Import aircraft (default "f16")
  if (!aircraft_fdmexec->LoadModel(aircraft_type_string)) {
    std::cerr << "Failed to load model \"" << aircraft_type_string << "\" from tmp_dir \`" << tmp_dir << "\`" << std::endl;
    return nullptr;
  }

  POP_JSBSIM_DEBUG_LEVEL
  
  // Return unique_ptr
  return std::move(aircraft_fdmexec);
}

void load_aircraft_ic_config(
  std::shared_ptr<JSBSim::FGInitialCondition>& aircraft_ic,
  types::AircraftInitialConditionConfig& config,
  bool quiet
) {
  PUSH_JSBSIM_DEBUG_LEVEL
  aircraft_ic->InitializeIC();
  aircraft_ic->SetLatitudeDegIC(config.latitude_deg);
  aircraft_ic->SetLongitudeDegIC(config.longitude_deg);
  aircraft_ic->SetAltitudeASLFtIC(config.altitude_asl_ft);
  aircraft_ic->SetPhiDegIC(config.roll_deg);
  aircraft_ic->SetThetaDegIC(config.pitch_deg);
  aircraft_ic->SetPsiDegIC(config.heading_deg);
  aircraft_ic->SetVNorthFpsIC(config.true_airspeed_fps);
  POP_JSBSIM_DEBUG_LEVEL
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

std::string to_type_string(types::AircraftType& t) {
  switch (t) {
    case types::AircraftType::F16: return "f16";
    default: return NULL;
  }
}

Magnum::Vector3 as_magnum_RUB(float x, float y, float z) {
  return Magnum::Vector3{y, -z, -x};
}

Magnum::Vector3 as_jsbsim_NED(float x, float y, float z) {
  return Magnum::Vector3{-z, x, -y};
}

types::AircraftInitialConditionConfig fetch_preset(types::AircraftInitialConditionPreset preset) {
  switch (preset) {
    case types::AircraftInitialConditionPreset::DEFAULT: return {
      .altitude_asl_ft = 60.0f,
      .true_airspeed_fps = 250.0f,
      .roll_deg = -5.0f,
      .pitch_deg = 5.0f,
      .heading_deg = 0.0f,
      .latitude_deg = 0.0f,
      .longitude_deg = 0.0f,
    };
    case types::AircraftInitialConditionPreset::DEFAULT_OPPONENT: return {
      .altitude_asl_ft = 60.0f,
      .true_airspeed_fps = 250.0f,
      .roll_deg = 5.0f,
      .pitch_deg = 5.0f,
      .heading_deg = 180.0f,
      .latitude_deg = -1e-3f,
      .longitude_deg = 0.0f,
    };
    case types::AircraftInitialConditionPreset::ON_GROUND: return {
      .altitude_asl_ft = 5.6f,
      .true_airspeed_fps = 0.0f,
      .roll_deg = 0.0f,
      .pitch_deg = 0.0f,
      .heading_deg = 0.0f,
      .latitude_deg = 0.0f,
      .longitude_deg = 0.0f,
    };
    case types::AircraftInitialConditionPreset::TAKEOFF_ROLL: return {
      .altitude_asl_ft = 5.6f,
      .true_airspeed_fps = 20.0f,
      .roll_deg = 0.0f,
      .pitch_deg = 0.0f,
      .heading_deg = 0.0f,
      .latitude_deg = 0.0f,
      .longitude_deg = 0.0f,
    };
    case types::AircraftInitialConditionPreset::TAKEOFF_ROLL_ROTATION: return {
      .altitude_asl_ft = 5.6f,
      .true_airspeed_fps = 245.0f,
      .roll_deg = 0.0f,
      .pitch_deg = 15.0f,
      .heading_deg = 0.0f,
      .latitude_deg = -2e-3f,
      .longitude_deg = 0.0f,
    };
    default: return {};
  }
}

}
