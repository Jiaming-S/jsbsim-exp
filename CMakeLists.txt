cmake_minimum_required(VERSION 3.16)
project(jsbsimexp VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Look for SDL2
find_package(SDL2 REQUIRED)

# Setup FetchContent
include(FetchContent)

# Fetch magic_enum
FetchContent_Declare(
  magic_enum
  GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
  GIT_TAG        v0.9.5
)

FetchContent_MakeAvailable(magic_enum)

# Fetch JSBSim
FetchContent_Declare(
  jsbsim_fetch
  GIT_REPOSITORY https://github.com/JSBSim-Team/jsbsim.git
  GIT_TAG        master 
)

# Disable JSBSim features we don't need
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_PYTHON_MODULE OFF CACHE BOOL "" FORCE)
set(BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(jsbsim_fetch)

# Fetch ZeroMQ
FetchContent_Declare(
  zeromq_fetch
  GIT_REPOSITORY https://github.com/zeromq/libzmq.git
  GIT_TAG        v4.3.5
)

# Disable libzmq features we don't need
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ENABLE_DRAFTS OFF CACHE BOOL "" FORCE)
set(WITH_PERF_TOOL OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(zeromq_fetch)

# Fetch ZeroMQ C++ Bindings (cppzmq)
FetchContent_Declare(
  cppzmq_fetch
  GIT_REPOSITORY https://github.com/zeromq/cppzmq.git
  GIT_TAG        v4.11.0
)

# Disable cppzmq tests
set(CPPZMQ_BUILD_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(cppzmq_fetch)

# Fetch msgpack
FetchContent_Declare(
  msgpack_fetch
  GIT_REPOSITORY https://github.com/msgpack/msgpack-c.git
  GIT_TAG        cpp-8.0.0
)

# Disable msgpack-cxx features we don't need
set(MSGPACK_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(MSGPACK_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(MSGPACK_USE_BOOST OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(msgpack_fetch)

# Fetch Corrade
FetchContent_Declare(
  corrade_fetch
  GIT_REPOSITORY https://github.com/mosra/corrade.git
  GIT_TAG        master
)

set(CORRADE_WITH_INTERCONNECT OFF CACHE BOOL "" FORCE)
set(CORRADE_WITH_TESTSUITE OFF CACHE BOOL "" FORCE)
set(CORRADE_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(CORRADE_WITH_RC ON CACHE BOOL "Enable Corrade Resource Compiler" FORCE)

FetchContent_MakeAvailable(corrade_fetch)

# Fetch Magnum
FetchContent_Declare(
  magnum_fetch
  GIT_REPOSITORY https://github.com/mosra/magnum.git
  GIT_TAG        master
)

set(MAGNUM_WITH_SDL2APPLICATION ON CACHE BOOL "" FORCE)
set(MAGNUM_WITH_GL ON CACHE BOOL "" FORCE)
set(MAGNUM_WITH_SHADERS ON CACHE BOOL "" FORCE)
set(MAGNUM_WITH_PRIMITIVES ON CACHE BOOL "" FORCE)
set(MAGNUM_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(MAGNUM_WITH_TRADE ON CACHE BOOL "" FORCE)
set(MAGNUM_BUILD_PLUGINS_STATIC ON CACHE BOOL "" FORCE)
set(MAGNUM_WITH_ANYSCENEIMPORTER ON CACHE BOOL "Enable AnySceneImporter plugin" FORCE)
set(MAGNUM_WITH_ANYIMAGEIMPORTER ON CACHE BOOL "Enable AnyImageImporter plugin" FORCE)

FetchContent_MakeAvailable(magnum_fetch)

# Fetch ImGui
FetchContent_Declare(
  imgui_fetch
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        8936b58fe26e8c3da834b8f60b06511d537b4c63
)

FetchContent_MakeAvailable(imgui_fetch)
set(IMGUI_DIR "${imgui_fetch_SOURCE_DIR}" CACHE PATH "Path to ImGui" FORCE)

# Fetch Magnum Integration
FetchContent_Declare(
  magnum_integration_fetch
  GIT_REPOSITORY https://github.com/mosra/magnum-integration.git
  GIT_TAG        af4754a1cf736901626fdd8d7b8fd739f7a02112
)

set(MAGNUM_WITH_IMGUIINTEGRATION ON CACHE BOOL "Enable ImGui integration wtih Magnum" FORCE)

FetchContent_MakeAvailable(magnum_integration_fetch)

# Fetch Magnum Plugins
FetchContent_Declare(
  magnum_plugins_fetch
  GIT_REPOSITORY https://github.com/mosra/magnum-plugins.git
  GIT_TAG        master
)

set(MAGNUM_WITH_PNGIMPORTER ON CACHE BOOL "Enable PngImporter plugin" FORCE)
set(MAGNUM_WITH_TINYGLTFIMPORTER ON CACHE BOOL "Enable TinyGltfImporter plugin" FORCE)

FetchContent_MakeAvailable(magnum_plugins_fetch)

# Point to resources.conf enumeratings files to be packaged into binary
corrade_add_resource(app_resources resources.conf)

# Build and link jsbsimexp
add_executable(jsbsimexp 
  src/main.cpp
  src/app.cpp
  src/types/types.cpp
  src/aircrafthandle.cpp
  src/camerahandle.cpp
  src/model/model.cpp
  src/utils/utils.cpp
  src/components/aircraft_control_component.cpp
  src/components/aircraft_state_info_component.cpp
  src/components/camera_control_component.cpp
  src/components/camera_draw_component.cpp
  src/components/foreign_input_component.cpp
  src/components/gui_component.cpp
  src/components/hand_of_god_tick_component.cpp
  src/components/keyboard_input_component.cpp
  src/components/mouse_cursor_hide_component.cpp
  src/components/mouse_input_component.cpp
  src/components/scenario_control_component.cpp
  src/components/sim_tick_component.cpp
  src/components/telemetry_tick_component.cpp
  src/components/vis_tick_component.cpp
  ${app_resources}
)

target_include_directories(jsbsimexp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/types)

target_link_libraries(jsbsimexp PRIVATE 
  magic_enum::magic_enum
  libJSBSim 
  cppzmq
  msgpack-cxx
  Magnum::Magnum 
  Magnum::Sdl2Application 
  Magnum::GL
  Magnum::Shaders
  Magnum::Primitives
  Magnum::SceneGraph
  Magnum::Trade
  MagnumIntegration::ImGui
  AnySceneImporter
  PngImporter
  TinyGltfImporter
  Corrade::Utility
  SDL2::SDL2
)

target_compile_options(jsbsimexp PRIVATE -Wall -Werror -O3 -g) # TODO: remove debug
target_compile_features(jsbsimexp PRIVATE cxx_std_17)
