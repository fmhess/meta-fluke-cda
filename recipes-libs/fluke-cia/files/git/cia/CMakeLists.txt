#-------------------------------------------------
#
# Copyright (c) 2019 Fluke Corporation, Inc. All rights reserved.
# Use of the software source code and warranty disclaimers are
# identified in the Software Agreement associated herewith.
#
# Repository URL:    git@git.sesg.fluke.com:fcal/cia
# Authored By:       B.J.Araujo
# Origin:            CIA
#
# Project build file for the "CIA" project.
#
#-------------------------------------------------

cmake_minimum_required(VERSION 3.0)

project(CIA)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Nighthawk settings
set(THREADS_PTHREAD_ARG "2" CACHE STRING "Forcibly set by CMakeLists.txt." FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set(ENABLE_STRIP_DEBUG_INFO ON CACHE BOOL "Strip debug info")
set(ENABLE_REMOTE_TRACE OFF CACHE BOOL "Enable remote tracing")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(NOT WIN32)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated")
endif()

#find_package(PythonInterp 2)
#find_package(PythonInterp 3)
set(PYTHON2_EXECUTABLE "/usr/bin/python")
set(PYTHON3_EXECUTABLE "/usr/bin/python3")

set(CIA_DIR ${CMAKE_CURRENT_SOURCE_DIR})

if (CMAKE_SYSTEM_PROCESSOR STREQUAL CMAKE_HOST_SYSTEM_PROCESSOR)
    set(BUILD_UPDATER OFF CACHE BOOL "Build updater module")
    set(CIA_BIN_DIR "${CIA_DIR}/bin/host")
else()
    set(BUILD_UPDATER ON CACHE BOOL "Build updater module")
    set(CIA_BIN_DIR "${CIA_DIR}/bin/cda-arm")
endif()

# Define add_deployment_file so build doesn't break, I don't think
# we need it to do anything though.
macro(add_deployment_file SRC DEST)
#    file(APPEND "${CMAKE_SOURCE_DIR}/QtCreatorDeployment.txt" "${SRC}:${DEST}\n")
endmacro()

set(CIA_BIN_DIR "${CIA_DIR}/bin/cda-arm")
# CIA cmake build should not in any way depend on nighthawk variables, but it does...
set(NIGHTHAWK_BIN_DIR "${CMAKE_SOURCE_DIR}/bin/cda-arm")
set(NIGHTHAWK_BIN_DIR_TARGET "/home/Nighthawk/bin")
set(NIGHTHAWK_TEMP_DIR_TARGET "/tmp")

add_definitions(-DTARGET_BUILD)

install(FILES "${CIA_DIR}/src/ogsrc/error/cia_error.json" DESTINATION ${CMAKE_INSTALL_PREFIX}/Nighthawk/bin/error)
install(FILES "${CIA_DIR}/src/ogsrc/remote/remote_error.json" DESTINATION ${CMAKE_INSTALL_PREFIX}/Nighthawk/bin/error)
install(FILES "${CIA_DIR}/src/ogsrc/settings/systemsettings.json" DESTINATION ${CMAKE_INSTALL_PREFIX}/Nighthawk/bin/settingsNV)

add_subdirectory(thirdparty)
add_subdirectory(src)




