#pragma once

#include "Magnum/Magnum.h"
#include <Magnum/GL/Mesh.h>
#include <Magnum/SceneGraph/Object.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>

#include <msgpack.hpp>

#include <FGFDMExec.h>


namespace types {

using Object3D = Magnum::SceneGraph::Object<Magnum::SceneGraph::MatrixTransformation3D>;
using Scene3D = Magnum::SceneGraph::Scene<Magnum::SceneGraph::MatrixTransformation3D>;

struct Object3DRenderable {
  types::Object3D *node;
  Magnum::GL::Mesh *mesh;
  Magnum::GL::Texture2D* texture;
};

enum AircraftType {
  F16,
  F16_NO_PID,
};

enum AircraftInitialConditionPreset {
  DEFAULT,
  DEFAULT_OPPONENT,
  ON_GROUND,
  TAKEOFF_ROLL,
  TAKEOFF_ROLL_ROTATION,
  LEFT_SPIRAL,
  RIGHT_SPIRAL,
  LEFT_TAXI,
  RIGHT_TAXI,
};

// Note: JSBSim uses ft/fps, km/kph is just for gui
struct AircraftStateInfo {
  Magnum::Rad pitch_rad{0.0};
  Magnum::Rad roll_rad{0.0};
  Magnum::Rad yaw_rad{0.0};
  Magnum::Deg pitch_deg{0.0};
  Magnum::Deg roll_deg{0.0};
  Magnum::Deg yaw_deg{0.0};
  Magnum::Rad alpha_rad;
  Magnum::Deg alpha_deg;
  double alt_ft = 0.0;
  double north_ft = 0.0;
  double east_ft = 0.0;
  double down_ft = 0.0;
  double alt_km = 0.0;
  double north_km = 0.0;
  double east_km = 0.0;
  double down_km = 0.0;
  double north_spd_fps = 0.0;
  double east_spd_fps = 0.0;
  double down_spd_fps = 0.0;
  double ground_spd_fps = 0.0;
  double indicated_spd_fps = 0.0;
  double north_spd_kph = 0.0;
  double east_spd_kph = 0.0;
  double down_spd_kph = 0.0;
  double ground_spd_kph = 0.0;
  double indicated_spd_kph = 0.0;
};

struct AircraftTrailBreadcrumb {
  AircraftStateInfo breadcrumb;
  double sim_time;
};

enum AircraftKeyPoints {
  NOSE,
  WINGTIP_L,
  WINGTIP_R,
  ENGINE_EXHAUST,
};


// Blackboard Types

// Info on our physics simulation type
enum eSimPhysicsState {
  NORMAL,
  PAUSED,
  CUSTOM,
};

// Info on whether we are controlling the camera or model
enum eSimControlType {
  CAMERA,
  MODEL,
};

// Info on whether we are orientation-locked or free floating about camera root
enum eCameraType {
  LOCKED,
  FREE,
};

// Info on whether there is an active aircraft
enum eAircraftActive {
  NO_ACTIVE,
  HAS_ACTIVE,
};

// Info on whether cursor is hidden or visible
enum eCursorHidden {
  HIDDEN_AND_LOCKED,
  VISIBLE,
};

// Info on whether debug information is displayed
enum eDebugViews {
  DEBUG_NOT_VISIBLE,
  DEBUG_OBJECTS_AND_STATISICS,
  DEBUG_OBJECTS,
  DEBUG_STATISTICS,
};

// Info on whether telemetry is currently sending
enum eTelemetryMode {
  TELEMETRY_ON,
  TELEMETRY_OFF,
};

// Info on whether telemetry was sent successfully (or no status)
enum eTelemetryDeliveryStatus {
  PACKET_NO_STATUS,
  PACKET_SEND_SUCCESS,
  PACKET_SEND_FAILURE,
};


// Telemetry Blackboard types

// Telemetry for each AircraftHandle
struct AircraftHandleTelemetry {
  float pitch_rad;
  float roll_rad;
  float yaw_rad;
  float alpha_rad;
  double alt_ft;
  double north_ft;
  double east_ft;
  double down_ft;
  double north_spd_fps;
  double east_spd_fps;
  double down_spd_fps;
  double ground_spd_fps;
  double indicated_spd_fps;

  MSGPACK_DEFINE(
    pitch_rad,
    roll_rad,
    yaw_rad,
    alpha_rad,
    alt_ft,
    north_ft,
    east_ft,
    down_ft,
    north_spd_fps,
    east_spd_fps,
    down_spd_fps,
    ground_spd_fps,
    indicated_spd_fps
  );
};

// Telemetry for commanded inputs
struct InputTelemetry {
  float commanded_aircraft_throttle;
  float commanded_aircraft_braking;
  float commanded_aircraft_roll;
  float commanded_aircraft_yaw;
  float commanded_aircraft_pitch;

  MSGPACK_DEFINE(
    commanded_aircraft_throttle,
    commanded_aircraft_braking,
    commanded_aircraft_roll,
    commanded_aircraft_yaw,
    commanded_aircraft_pitch
  );
};

};
