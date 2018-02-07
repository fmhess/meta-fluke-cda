FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

# revert bad patch from meta-qt5 layer, for more info see:
# https://github.com/meta-qt5/meta-qt5/commit/bf7cdd963008f2d61bc4c324fff2bfafe511fb5f
SRC_URI += " \
    file://0012REVERT-qdbuscpp2xml.pro-do-not-build-with-bootstrapped-depe.patch \
"
