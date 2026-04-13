cmake_minimum_required(VERSION 3.16)
project(jsbsimexp VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Look for SDL2
find_package(SDL2 REQUIRED)

# Setup FetchContent
include(FetchContent)

# Fetch JSBSim
FetchContent_Declare(
  jsbsim_fetch
  GIT_REPOSITORY https://github.com/JSBSim-Team/jsbsim.git
  GIT_TAG        master 
)

# Disable JSBSim features we don't need
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Disable JSBSim shared libs" FORCE)
set(BUILD_PYTHON_MODULE OFF CACHE BOOL "Disable JSBSim Python" FORCE)
set(BUILD_DOCS OFF CACHE BOOL "Disable JSBSim Docs" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "Disable JSBSim Tests" FORCE)

FetchContent_MakeAvailable(jsbsim_fetch)

# Fetch Corrade
FetchContent_Declare(
  corrade_fetch
  GIT_REPOSITORY https://github.com/mosra/corrade.git
  GIT_TAG        master
)

set(CORRADE_WITH_INTERCONNECT OFF CACHE BOOL "Disable Corrade Interconnect" FORCE)
set(CORRADE_WITH_TESTSUITE OFF CACHE BOOL "Disable Corrade TestSuite" FORCE)
set(CORRADE_BUILD_TESTS OFF CACHE BOOL "Disable Corrade Tests" FORCE)

FetchContent_MakeAvailable(corrade_fetch)

# Fetch Magnum
FetchContent_Declare(
  magnum_fetch
  GIT_REPOSITORY https://github.com/mosra/magnum.git
  GIT_TAG        master
)

set(MAGNUM_WITH_SDL2APPLICATION ON CACHE BOOL "Enable SDL2 Application" FORCE)
set(MAGNUM_WITH_GL ON CACHE BOOL "Enable OpenGL Wrapper" FORCE)
set(MAGNUM_WITH_SHADERS ON CACHE BOOL "Enable Built-in Shaders" FORCE)
set(MAGNUM_WITH_PRIMITIVES ON CACHE BOOL "Enable 3D Primitives" FORCE)
set(MAGNUM_BUILD_TESTS OFF CACHE BOOL "Disable Magnum Tests" FORCE)

FetchContent_MakeAvailable(magnum_fetch)

# Build and link jsbsimexp executable
add_executable(jsbsimexp 
  src/main.cpp
  src/sim.cpp
  src/utils.cpp
)

target_include_directories(jsbsimexp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/types)

# Link against libJSBSim and Magnum::*
target_link_libraries(jsbsimexp PRIVATE 
  libJSBSim 
  Magnum::Magnum 
  Magnum::Sdl2Application 
  Magnum::GL
  Magnum::Shaders
  Magnum::Primitives
  Magnum::SceneGraph
  SDL2::SDL2
)

target_compile_options(jsbsimexp PRIVATE -Os)
target_compile_features(jsbsimexp PRIVATE cxx_std_17)