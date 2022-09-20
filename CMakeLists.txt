cmake_minimum_required(VERSION 3.7)

project(AGRAPHICS
  VERSION 0.1.0
  LANGUAGES C
)

if(NOT DEFINED CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
  message(STATUS "Setting build type to 'RelWithDebInfo' as none was specified.")
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS FALSE)

if(UNIX)
  set(CMAKE_C_FLAGS "-Wall")
endif()

include(GNUInstallDirs)

find_package(PkgConfig REQUIRED)
pkg_check_modules(CAIRO REQUIRED cairo>=1.12 cairo-png>=1.12)

set(AGRAPHICS_UNIT_DIRECTORY "${CMAKE_INSTALL_PREFIX}/share/agraphics")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config.h.in" "${CMAKE_CURRENT_BINARY_DIR}/config.h" @ONLY)

add_executable(agraphics
  agraphics.c
  agate/agate.c
  agate/support.c
)

target_include_directories(agraphics
  PRIVATE
    "${CAIRO_INCLUDE_DIRS}"
    "${CMAKE_CURRENT_SOURCE_DIR}/agate"
    "${CMAKE_CURRENT_BINARY_DIR}"
)

target_link_libraries(agraphics
  PRIVATE
    m
    ${CAIRO_LIBRARIES}
)

install(
  TARGETS agraphics
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

install(
  FILES agraphics.agate
  DESTINATION "${CMAKE_INSTALL_PREFIX}/share/agraphics"
)
