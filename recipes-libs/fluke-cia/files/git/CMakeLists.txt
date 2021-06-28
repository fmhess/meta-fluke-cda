#-------------------------------------------------
#
# Copyright (c) 2021 Fluke Corporation, Inc. All rights reserved.
# Use of the software source code and warranty disclaimers are
# identified in the Software Agreement associated herewith.
#
# Top level build file used by fluke-cia-libs_git.bb Yocto
# recipe to do a standalone build of the "CIA" project.
#
#-------------------------------------------------

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

add_subdirectory(cia)
