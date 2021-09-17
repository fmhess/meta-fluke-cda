#patch to fix error when compiling binutils from Zeus with gcc 10

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

SRC_URI_append = "\
     file://1000-fix-gcc10-compile.patch \
"
