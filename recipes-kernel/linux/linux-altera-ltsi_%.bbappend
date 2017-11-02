PR = "r205"
KERNEL_REPO = "git://github.com/fmhess/linux-socfpga.git"
LINUX_VERSION_SUFFIX = "-ltsi-${MACHINE}"
SRCREV = "${AUTOREV}"

# KERNEL_DEVICETREE_fluke-cda-richmond = "Richmond_005.dtb Richmond_005_overlay.dtb"
KERNEL_DEVICETREE_fluke-cda-vanquish = "Vanquish_soc.dtb Vanquish_soc_overlay.dtb"
KERNEL_DEVICETREE_fluke-cda-richmond = "Richmond_005.dtb Richmond_005_overlay.dtb"
KERNEL_DEVICETREE_fluke-cda-triclamp = "Triclamp_soc.dtb Triclamp_soc_overlay.dtb"
KERNEL_DEVICETREE_fluke-cda-nighthawk = "Nighthawk_soc.dtb Nighthawk_soc_overlay.dtb" 

FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

#  file://0005-Kludge-to-get-around-IRQF_DISABLED-going-away.patch

SRC_URI_append = "\
    file://fluke-cda.cfg \
"
    
# SRC_URI_append = "\
#     file://0001-Driver-support-for-Altera-interval-timers.patch \
#     file://0002-Driver-support-for-Fluke-GPIO-devices.patch \
#     file://0003-Driver-support-for-GPIB-device-support.patch \
#     file://0002-added-ifc-event-support.patch \
#     file://0003-Fixed-typo.patch \
#     file://0004-Fixed-compile-error.patch \
#     file://0005-GPIB-fixed-corner-case-where-EOI-could-become-decoup.patch \
#     file://0006-GPIB-need-to-manually-clear-the-register-page-in-pag.patch \
#     file://0007-GPIB-cleaned-up-some-unneeded-paranoia-code.patch \
#     file://0008-GPIB-after-updating-CMPL-return-updated-value-instea.patch \
#     file://0004-Driver-modification-dmaengine-to-fix-get_slave_ch.patch \
#     file://0005-Driver-modification-spidev-allow-generic-spidev.patch \
#     file://0025-Altera-base-layer-socfpga_dtsi-is-decode-equal-0.patch \
#     file://fluke-cda.cfg \
# "
# 
# SRC_URI_append_fluke-cda-vanquish = "\
#     file://0009-Add-Device-Tree-for-Vanquish.patch \
#     file://0010-Add-Device-Tree-Overlay-for-Vanquish.patch \
#     file://0017-DeviceTree-modification-Vanquish-spi-temp-sensor.patch \
#     file://0034-Vanquish-Qspi-decode-mode-select-fix.patch \
#     file://0020-Added-driver-ioctl_8250_fast_uart-header-file.patch \
#     file://0021-Modified-8250-uart-pl330-dma-for-High-speed-uart.patch \
#     file://0022-modified-driver-edmengine.h-serial_8250.h-HS-uart.patch \
#     file://0033-HS-uart-clean-up-0ct-2016.patch \
#     file://0035-Vanquish_DTS-reduce-qspi-flash-clocks-to-50MHZ.patch \
#     file://0078-Added-Rohde-Schwarz-USB-NRP-power-meter-driver.patch \
#     file://0080-Vanquish-dts-added-PMU-support-1-core-only.patch \
#     file://0081-Vanquish-dts-fix-for-pmu-error-1.patch \
#     file://0082-Vanquish-dts-fix-for-pmu-error-2.patch \
#     file://0083-Vanquish-dts-and-socfpga-dtsi-fix-for-pmu-error-3.patch \
#     file://0084-Added-initial-entry-for-Micron-2G-flash.patch \
# "
# SRC_URI_append_fluke-cda-richmond = "\
#     file://0011-Add-Device-Tree-for-Richmond_005.patch \
#     file://0012-Add-Device-Tree-Overlay-for-Richmod_005.patch \
#     file://0014-DeviceTreeOverlay-modification-Richmond-fgpio.patch \
#     file://0015-DeviceTreeOverlay-modification-Richmond-fgpio-2.patch \
#     file://0016-DeviceTreeOverlay-modification-Richmond-fgpib-3.patch \
#     file://0013-Driver-modification-c_can-use-32-bit-Read-Write.patch \
#     file://0018-gpio-debug-patch-do-not-include-with-release.patch \
#     file://0036-Richmond_005_DTS-reduce-qspi-flash-clocks-to-50MHZ.patch \
# "
# SRC_URI_append_fluke-cda-triclamp = "\
#     file://0007-Add-Device-Tree-for-TriClamp.patch \
#     file://0027-Triclamp_DTS-reduce-qspi-flash-clocks-to-50MHZ.patch \
#     file://0031-added-m25-Fast-Read-to-both-memory-structures.patch \
#     file://0032-removed-fast-mode-from-qspi-structs.patch \
#     file://0008-Add-Device-Tree-Overlay-for-TriClamp.patch \
#     file://0019-Driver-modification-fluke-matrix-keypad-Kconfig.patch \
#     file://0006-Driver-support-for-fluke-matrix-keypad.patch \
# "
# SRC_URI_append_fluke-cda-nighthawk = "\
#     file://0037-Add-Nighthawk_soc.dts-file.patch \
#     file://0038-Nighthawk-device-tree-syntax-fix.-1.patch \
#     file://0039-Nighthawk-device-tree-syntax-fix.-2.patch \
#     file://0040-Nighthawk-device-tree-syntax-fix.-3.patch \
#     file://0041-Nighthawk-device-tree-syntax-fix.-4.patch \
#     file://0042-Nighthawk-device-tree-syntax-fix.-5.patch \
#     file://0043-Nighthawk-device-tree-syntax-fix.-r6.patch \
#     file://0044-Nighthawk-device-tree-syntax-fix.-7.patch \
#     file://0045-Nighthawk-device-tree-syntax-fix.-8.patch.patch \
#     file://0046-Nighthawk-device-tree-syntax-fix.-9.patch \
#     file://0047-Nighthawk-device-tree-syntax-fix.-10.patch \
#     file://0048-added-the-Nighthawk-device-tree-overlay.patch \
#     file://0049-Nighthawk-soc-overlay-fix-1.patch \
#     file://0050-Nighthawk-soc-overlay-fix-2.patch \
#     file://0051-Nighthawk-soc-overlay-fix-3.patch \
#     file://0052-Nighthawk-device-tree-syntax-fix.11.patch \
#     file://0053-Nighthawk-device-tree-syntax-fix.12.patch \
#     file://0054-Nighthawk-soc-overlay-fix-4.patch \
#     file://0055-ghthawk-device-tree-syntax-fix.13.patch \
#     file://0056-Nighthawk-device-tree-syntax-fix.14.patch \
#     file://0057-Nighthawk-device-tree-syntax-fix.15.patch \
#     file://0058-Nighthawk-device-tree-syntax-fix.16.patch \
#     file://0059-Nighthawk-device-tree-syntax-fix.17.patch \
#     file://0060-Nighthawk-device-tree-syntax-fix.18.patch \
#     file://0061-Nighthawk-device-tree-syntax-fix.19.patch \
#     file://0062-Nighthawk-device-tree-syntax-fix.20.patch \
#     file://0063-Nighthawk-device-tree-syntax-fix.21.patch \
#     file://0064-Nighthawk-device-tree-syntax-fix.22.patch \
#     file://0065-Nighthawk-device-tree-syntax-fix.23.patch \
#     file://0066-Nighthawk-device-tree-syntax-fix.24.patch \
#     file://0067-Nighthawk-device-tree-syntax-fix.25.patch \
#     file://0068-Nighthawk-device-tree-syntax-fix.26.patch \
#     file://0069-Nighthawk-device-tree-syntax-fix.27.patch \
#     file://0070-Nighthawk-device-tree-syntax-fix.27.patch \
#     file://0071-Nighthawk-device-tree-syntax-fix.28.patch \
#     file://0072-Nighthawk-device-tree-syntax-fix.29.patch \
#     file://0073-Nighthawk-device-tree-syntax-fix.30.patch \
#     file://0074-Nighthawk-device-tree-syntaxr-fix.31.patch \
#     file://0075-Nighthawk-device-tree-syntax-fix.32.patch \
#     file://0076-Nighthawk-device-tree-syntax-fix.33.patch \
#     file://0077-Device-Tree-Qspi-partition-fixes-flash-0-flash-1.patch \
#     file://0079-Nighthawk-device-tree-syntaxr-fix.34.patch \
#     file://0087-Nighthawk-rgmii-to-gmii-on-eth0.patch \
#     file://0088-Nighthawk-rgmii-to-gmii-on-eth1.patch \
#     file://0089-Nighthawk-add-ethernet-aliases-to-DT.patch \
#     file://0090-Nighthawk-fix-ethernet-aliases-to-DT.patch \
#     file://0091-Nighthawk-stmmac-socfpga-remove-extra-call-to-socfpga_dwmac_se.patch \
# "
