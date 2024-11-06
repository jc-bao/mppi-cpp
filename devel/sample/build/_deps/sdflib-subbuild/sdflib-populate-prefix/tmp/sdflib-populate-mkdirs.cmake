# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "C:/Users/JC-Ba/Documents/Research/code/mppi-cpp/devel/sample/build/_deps/sdflib-src")
  file(MAKE_DIRECTORY "C:/Users/JC-Ba/Documents/Research/code/mppi-cpp/devel/sample/build/_deps/sdflib-src")
endif()
file(MAKE_DIRECTORY
  "C:/Users/JC-Ba/Documents/Research/code/mppi-cpp/devel/sample/build/_deps/sdflib-build"
  "C:/Users/JC-Ba/Documents/Research/code/mppi-cpp/devel/sample/build/_deps/sdflib-subbuild/sdflib-populate-prefix"
  "C:/Users/JC-Ba/Documents/Research/code/mppi-cpp/devel/sample/build/_deps/sdflib-subbuild/sdflib-populate-prefix/tmp"
  "C:/Users/JC-Ba/Documents/Research/code/mppi-cpp/devel/sample/build/_deps/sdflib-subbuild/sdflib-populate-prefix/src/sdflib-populate-stamp"
  "C:/Users/JC-Ba/Documents/Research/code/mppi-cpp/devel/sample/build/_deps/sdflib-subbuild/sdflib-populate-prefix/src"
  "C:/Users/JC-Ba/Documents/Research/code/mppi-cpp/devel/sample/build/_deps/sdflib-subbuild/sdflib-populate-prefix/src/sdflib-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/JC-Ba/Documents/Research/code/mppi-cpp/devel/sample/build/_deps/sdflib-subbuild/sdflib-populate-prefix/src/sdflib-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/JC-Ba/Documents/Research/code/mppi-cpp/devel/sample/build/_deps/sdflib-subbuild/sdflib-populate-prefix/src/sdflib-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
