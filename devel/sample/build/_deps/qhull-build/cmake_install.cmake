# Install script for directory: /home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src

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

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/qhull" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/qhull")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/qhull"
         RPATH "lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/home/pcy/Research/code/mppi-cpp/devel/sample/build/bin/qhull")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/qhull" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/qhull")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/qhull"
         OLD_RPATH ":::"
         NEW_RPATH "lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/llvm-strip-12" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/qhull")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/rbox" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/rbox")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/rbox"
         RPATH "lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/home/pcy/Research/code/mppi-cpp/devel/sample/build/bin/rbox")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/rbox" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/rbox")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/rbox"
         OLD_RPATH ":::"
         NEW_RPATH "lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/llvm-strip-12" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/rbox")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/qconvex" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/qconvex")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/qconvex"
         RPATH "lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/home/pcy/Research/code/mppi-cpp/devel/sample/build/bin/qconvex")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/qconvex" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/qconvex")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/qconvex"
         OLD_RPATH ":::"
         NEW_RPATH "lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/llvm-strip-12" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/qconvex")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/qdelaunay" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/qdelaunay")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/qdelaunay"
         RPATH "lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/home/pcy/Research/code/mppi-cpp/devel/sample/build/bin/qdelaunay")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/qdelaunay" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/qdelaunay")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/qdelaunay"
         OLD_RPATH ":::"
         NEW_RPATH "lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/llvm-strip-12" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/qdelaunay")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/qvoronoi" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/qvoronoi")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/qvoronoi"
         RPATH "lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/home/pcy/Research/code/mppi-cpp/devel/sample/build/bin/qvoronoi")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/qvoronoi" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/qvoronoi")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/qvoronoi"
         OLD_RPATH ":::"
         NEW_RPATH "lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/llvm-strip-12" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/qvoronoi")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/qhalf" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/qhalf")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/qhalf"
         RPATH "lib")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/home/pcy/Research/code/mppi-cpp/devel/sample/build/bin/qhalf")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/qhalf" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/qhalf")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/qhalf"
         OLD_RPATH ":::"
         NEW_RPATH "lib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/llvm-strip-12" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/qhalf")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/pcy/Research/code/mppi-cpp/devel/sample/build/lib/libqhullcpp.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/pcy/Research/code/mppi-cpp/devel/sample/build/lib/libqhullstatic.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/pcy/Research/code/mppi-cpp/devel/sample/build/lib/libqhullstatic_r.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/QHull/QhullTargets.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/QHull/QhullTargets.cmake"
         "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-build/CMakeFiles/Export/lib/cmake/QHull/QhullTargets.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/QHull/QhullTargets-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/QHull/QhullTargets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/QHull" TYPE FILE FILES "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-build/CMakeFiles/Export/lib/cmake/QHull/QhullTargets.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/QHull" TYPE FILE FILES "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-build/CMakeFiles/Export/lib/cmake/QHull/QhullTargets-release.cmake")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDevelx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/QHull" TYPE FILE FILES
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-build/QhullExport/QhullConfig.cmake"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-build/QhullExport/QhullConfigVersion.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDevelx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-build/qhull_r.pc")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDevelx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-build/qhullstatic.pc")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDevelx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-build/qhullstatic_r.pc")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xDevelx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-build/qhullcpp.pc")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/libqhull" TYPE FILE FILES
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhull/libqhull.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhull/geom.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhull/io.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhull/mem.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhull/merge.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhull/poly.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhull/qhull_a.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhull/qset.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhull/random.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhull/stat.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhull/user.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/libqhull" TYPE FILE FILES "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhull/DEPRECATED.txt")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/libqhull_r" TYPE FILE FILES
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhull_r/libqhull_r.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhull_r/geom_r.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhull_r/io_r.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhull_r/mem_r.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhull_r/merge_r.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhull_r/poly_r.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhull_r/qhull_ra.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhull_r/qset_r.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhull_r/random_r.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhull_r/stat_r.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhull_r/user_r.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/libqhullcpp" TYPE FILE FILES
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhullcpp/Coordinates.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhullcpp/functionObjects.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhullcpp/PointCoordinates.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhullcpp/Qhull.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhullcpp/QhullError.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhullcpp/QhullFacet.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhullcpp/QhullFacetList.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhullcpp/QhullFacetSet.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhullcpp/QhullHyperplane.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhullcpp/QhullIterator.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhullcpp/QhullLinkedList.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhullcpp/QhullPoint.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhullcpp/QhullPoints.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhullcpp/QhullPointSet.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhullcpp/QhullQh.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhullcpp/QhullRidge.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhullcpp/QhullSet.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhullcpp/QhullSets.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhullcpp/QhullStat.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhullcpp/QhullUser.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhullcpp/QhullVertex.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhullcpp/QhullVertexSet.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhullcpp/RboxPoints.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhullcpp/RoadError.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/libqhullcpp/RoadLogEvent.h"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/qhulltest/RoadTest.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/man/man1" TYPE FILE RENAME "qhull.1" FILES "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/html/qhull.man")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/man/man1" TYPE FILE RENAME "rbox.1" FILES "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/html/rbox.man")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/doc/qhull" TYPE FILE FILES
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/README.txt"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/REGISTER.txt"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/Announce.txt"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/COPYING.txt"
    "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/index.htm"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/doc/qhull/html" TYPE DIRECTORY FILES "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/html/")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/doc/qhull/src" TYPE FILE FILES "/home/pcy/Research/code/mppi-cpp/devel/sample/build/_deps/qhull-src/src/Changes.txt")
endif()

