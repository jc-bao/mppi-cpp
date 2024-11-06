# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "C:/Users/JC-Ba/Documents/Research/code/mppi-cpp/devel/sample/build/_deps/spdlog_lib-src")
  file(MAKE_DIRECTORY "C:/Users/JC-Ba/Documents/Research/code/mppi-cpp/devel/sample/build/_deps/spdlog_lib-src")
endif()
file(MAKE_DIRECTORY
  "C:/Users/JC-Ba/Documents/Research/code/mppi-cpp/devel/sample/build/_deps/spdlog_lib-build"
  "C:/Users/JC-Ba/Documents/Research/code/mppi-cpp/devel/sample/build/_deps/spdlog_lib-subbuild/spdlog_lib-populate-prefix"
  "C:/Users/JC-Ba/Documents/Research/code/mppi-cpp/devel/sample/build/_deps/spdlog_lib-subbuild/spdlog_lib-populate-prefix/tmp"
  "C:/Users/JC-Ba/Documents/Research/code/mppi-cpp/devel/sample/build/_deps/spdlog_lib-subbuild/spdlog_lib-populate-prefix/src/spdlog_lib-populate-stamp"
  "C:/Users/JC-Ba/Documents/Research/code/mppi-cpp/devel/sample/build/_deps/spdlog_lib-subbuild/spdlog_lib-populate-prefix/src"
  "C:/Users/JC-Ba/Documents/Research/code/mppi-cpp/devel/sample/build/_deps/spdlog_lib-subbuild/spdlog_lib-populate-prefix/src/spdlog_lib-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/JC-Ba/Documents/Research/code/mppi-cpp/devel/sample/build/_deps/spdlog_lib-subbuild/spdlog_lib-populate-prefix/src/spdlog_lib-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/JC-Ba/Documents/Research/code/mppi-cpp/devel/sample/build/_deps/spdlog_lib-subbuild/spdlog_lib-populate-prefix/src/spdlog_lib-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
