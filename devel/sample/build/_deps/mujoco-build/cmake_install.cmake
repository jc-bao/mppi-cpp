# Install script for directory: /home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/mujoco-src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/llvm-objdump-12")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/mujoco-build/plugin/elasticity/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/mujoco-build/plugin/actuator/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/mujoco-build/plugin/sensor/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/mujoco-build/plugin/sdf/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/mujoco-build/src/engine/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/mujoco-build/src/user/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/mujoco-build/src/xml/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/mujoco-build/src/render/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/mujoco-build/src/thread/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/mujoco-build/src/ui/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/mujoco-build/model/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/mujoco-build/simulate/cmake_install.cmake")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xruntimex" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libmujoco.so.3.2.6" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libmujoco.so.3.2.6")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libmujoco.so.3.2.6"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/pcy/Research/code/mppi-cpp/devel/sample/build/lib/libmujoco.so.3.2.6")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libmujoco.so.3.2.6" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libmujoco.so.3.2.6")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/llvm-strip-12" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libmujoco.so.3.2.6")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xruntimex" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libmujoco.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libmujoco.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libmujoco.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/home/pcy/Research/code/mppi-cpp/devel/sample/build/lib/libmujoco.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libmujoco.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libmujoco.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/llvm-strip-12" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libmujoco.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/mujoco" TYPE FILE FILES
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/mujoco-src/include/mujoco/mjdata.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/mujoco-src/include/mujoco/mjexport.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/mujoco-src/include/mujoco/mjmacro.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/mujoco-src/include/mujoco/mjmodel.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/mujoco-src/include/mujoco/mjplugin.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/mujoco-src/include/mujoco/mjrender.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/mujoco-src/include/mujoco/mjsan.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/mujoco-src/include/mujoco/mjspec.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/mujoco-src/include/mujoco/mjthread.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/mujoco-src/include/mujoco/mjtnum.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/mujoco-src/include/mujoco/mjui.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/mujoco-src/include/mujoco/mjvisualize.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/mujoco-src/include/mujoco/mjxmacro.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/mujoco-src/include/mujoco/mujoco.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/mujoco/mujocoTargets.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/mujoco/mujocoTargets.cmake"
         "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/mujoco-build/CMakeFiles/Export/lib/cmake/mujoco/mujocoTargets.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/mujoco/mujocoTargets-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/mujoco/mujocoTargets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/mujoco" TYPE FILE FILES "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/mujoco-build/CMakeFiles/Export/lib/cmake/mujoco/mujocoTargets.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/mujoco" TYPE FILE FILES "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/mujoco-build/CMakeFiles/Export/lib/cmake/mujoco/mujocoTargets-release.cmake")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/mujoco" TYPE FILE FILES
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/mujoco-build/mujocoConfig.cmake"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/mujoco-build/mujocoConfigVersion.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/mujoco" TYPE DIRECTORY FILES "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/mujoco-src/model" REGEX "/CMakeLists\\.txt$" EXCLUDE)
endif()

