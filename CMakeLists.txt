cmake_minimum_required(VERSION 3.16)
project(jsbsimexp VERSION 1.0 LANGUAGES C CXX)

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
  GIT_TAG        master
)

FetchContent_MakeAvailable(imgui_fetch)
set(IMGUI_DIR "${imgui_fetch_SOURCE_DIR}" CACHE PATH "Path to ImGui" FORCE)

# Fetch Magnum Integration
FetchContent_Declare(
  magnum_integration_fetch
  GIT_REPOSITORY https://github.com/mosra/magnum-integration.git
  GIT_TAG        master
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
  src/aircrafthandle.cpp
  src/camerahandle.cpp
  src/input/input.cpp
  src/model/model.cpp
  src/model/coloreddrawable.cpp
  src/model/textureddrawable.cpp
  src/utils/utils.cpp
  ${app_resources}
)

target_include_directories(jsbsimexp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/types)

target_link_libraries(jsbsimexp PRIVATE 
  magic_enum::magic_enum
  libJSBSim 
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

target_compile_options(jsbsimexp PRIVATE -Os -g) # TODO: remove debug
target_compile_features(jsbsimexp PRIVATE cxx_std_17)
