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

add_subdirectory(thirdparty)
add_subdirectory(src)




