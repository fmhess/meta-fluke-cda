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

set(ENABLE_REMOTE_TRACE OFF CACHE BOOL "Enable remote tracing")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(NOT WIN32)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated")
endif()

set(Python3_FIND_STRATEGY "LOCATION")
find_package (Python3 COMPONENTS Interpreter)

set(CIA_DIR ${CMAKE_CURRENT_SOURCE_DIR})

if (CMAKE_SYSTEM_PROCESSOR STREQUAL CMAKE_HOST_SYSTEM_PROCESSOR)
    set(BUILD_UPDATER OFF CACHE BOOL "Build updater module")
else()
    set(BUILD_UPDATER ON CACHE BOOL "Build updater module")
endif()

# Define add_deployment_file so build doesn't break, I don't think
# we need it to do anything though.
macro(add_deployment_file SRC DEST)
#    file(APPEND "${CMAKE_SOURCE_DIR}/QtCreatorDeployment.txt" "${SRC}:${DEST}\n")
endmacro()

set(CIA_BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}/bin/cda-arm")
# CIA cmake build should not in any way depend on nighthawk variables, but it does...
set(NIGHTHAWK_BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}/bin/cda-arm")
set(NIGHTHAWK_BIN_DIR_TARGET "/home/Nighthawk/bin")
set(NIGHTHAWK_TEMP_DIR_TARGET "/tmp")

add_definitions(-DTARGET_BUILD)

set(FOUND_LIBCIA_ANALOG CIA::analog CACHE STRING "" FORCE)
set(FOUND_LIBCIA_GXSERVERS CIA::gxservers CACHE STRING "" FORCE)
set(FOUND_LIBCIA_TRANSMISSION CIA::transmission CACHE STRING "" FORCE)
set(FOUND_LIBCIA_UTILS CIA::utils CACHE STRING "" FORCE)
set(FOUND_LIBCIA_PRINTSTREAM CIA::printstream CACHE STRING "" FORCE)
set(FOUND_LIBCIA_CONNECTORS CIA::connectors CACHE STRING "" FORCE)
set(FOUND_LIBCIA_ERROR CIA::error CACHE STRING "" FORCE)
set(FOUND_LIBCIA_LINEARIZE CIA::linearize CACHE STRING "" FORCE)
set(FOUND_LIBCIA_OPMAN CIA::opman CACHE STRING "" FORCE)
set(FOUND_LIBCIA_PROCEDURE CIA::procedure CACHE STRING "" FORCE)
set(FOUND_LIBCIA_REMOTE CIA::remote CACHE STRING "" FORCE)
set(FOUND_LIBCIA_JPAPARSER CIA::jpaparser CACHE STRING "" FORCE)
set(FOUND_LIBCIA_SETTINGS CIA::settings CACHE STRING "" FORCE)
set(FOUND_LIBCIA_SHARED CIA::shared CACHE STRING "" FORCE)
set(FOUND_LIBCIA_STATE CIA::state CACHE STRING "" FORCE)
set(FOUND_LIBCIA_TRACE CIA::trace CACHE STRING "" FORCE)
set(FOUND_LIBJSONRPCCPP_SERVER jsonrpccppserver CACHE STRING "" FORCE)
set(FOUND_LIBJSONRPCCPP_CLIENT jsonrpccppclient CACHE STRING "" FORCE)
set(FOUND_LIBJSONRPCCPP_COMMON jsonrpccppcommon CACHE STRING "" FORCE)
set(FOUND_LIBJSONCPP jsoncpp CACHE STRING "" FORCE)
set(FOUND_LIBUSBTMC usbtmc CACHE STRING "" FORCE)

add_subdirectory(thirdparty)
add_subdirectory(src)
