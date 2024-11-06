cmake_minimum_required(VERSION 3.16)

project(
  rs_cpp
  VERSION 0.1.0
  DESCRIPTION "Random Search"
  HOMEPAGE_URL "https://github.com/thowell/rs.cpp"
)

# Global compilation settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # For LLVM tooling

enable_language(C)
enable_language(CXX)
if(APPLE)
  enable_language(OBJC)
  enable_language(OBJCXX)
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include(FindOrFetch)

# MuJoCo
set(MUJOCO_GIT_TAG
    3.1.6
    CACHE STRING "Git revision for MuJoCo."
)

findorfetch(
  USE_SYSTEM_PACKAGE
  OFF
  PACKAGE_NAME
  mujoco
  LIBRARY_NAME
  mujoco
  GIT_REPO
  https://github.com/google-deepmind/mujoco.git
  GIT_TAG
  ${MUJOCO_GIT_TAG}
  TARGETS
  mujoco::mujoco
  mujoco::platform_ui_adapter
  EXCLUDE_FROM_ALL
)

# Abseil
set(RS_CPP_DEP_VERSION_abseil
    fb3621f4f897824c0dbe0615fa94543df6192f30 # LTS 20230802.1
    CACHE STRING "Version of `abseil` to be fetched."
    FORCE
)

set(ABSL_PROPAGATE_CXX_STD ON)
set(ABSL_BUILD_TESTING OFF)
# ABSL_ENABLE_INSTALL is needed for
# https://github.com/protocolbuffers/protobuf/issues/12185#issuecomment-1594685860
set(ABSL_ENABLE_INSTALL ON)
findorfetch(
  USE_SYSTEM_PACKAGE
  OFF
  PACKAGE_NAME
  absl
  LIBRARY_NAME
  abseil-cpp
  GIT_REPO
  https://github.com/abseil/abseil-cpp.git
  GIT_TAG
  ${RS_CPP_DEP_VERSION_abseil}
  TARGETS
  absl::core_headers
  EXCLUDE_FROM_ALL
)

# random search
add_executable(rs rs.cpp)

target_link_libraries(
  rs
  absl::random_random
  glfw
  mujoco::mujoco
)
