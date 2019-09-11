
TOOLCHAIN_HOST_TASK_append = " nativesdk-packagegroup-fluke-cda-toolchain-host"
TOOLCHAIN_TARGET_TASK_append = " packagegroup-fluke-cda-toolchain-target"

#SDK_DEPLOY is from layers/openembedded-core/meta/classes/populate_sdk_base.bbclass
#we add a SDKMACHINE and MACHINE subdirs to prevent breakage when rebuilding toolchain after changing MACHINE
SDK_DEPLOY = "${DEPLOY_DIR}/sdk/${SDKMACHINE}/${MACHINE}"
