SUMMARY = "Fluke CDA customization for Nighthawk"
DESCRIPTION = "Nighthawk application, etc."
AUTHOR = "Fluke"
SECTION = ""
HOMEPAGE = ""

LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://LICENSE;md5=f19e4f8ee79c4b8ff1fbb449f7e56c54"

#PR = "r0"

#fontconfig, freetype, libpng, libcrypto (provided by openssl) used by "/home/Nighthawk/bin/update" executable
#linux-gpib-user used by "/home/Nighthawk/bin/remoteapp" executable
#qtbase, qtdeclarative used by "/home/Nighthawk/bin/fp" executable
DEPENDS = " \
	fontconfig \
	freetype \
	libpng \
	openssl \
	linux-gpib-user \
	qtbase \
	qtdeclarative \
 "

SRC_URI = "git://github.com/ADorchak/sumo-rootfs-extras.git;protocol=https;branch=master"

SRCREV ?= "${AUTOREV}"

FILES_${PN} += " \
	${base_prefix}/www/* \
	${base_prefix}/home/Nighthawk/* \
	${base_prefix}/home/setup_environment_target \
"

S = "${WORKDIR}/git"

do_compile[noexec] = "1"

do_clean[noexec] = "1"

do_install () {
	#sudo ln -s /dev/null ./target/etc/systemd/network/10-en.network
	#sudo ln -s /dev/null ./target/etc/systemd/network/11-eth.network

	(
		cd ${S}/Nighthawk/extraFiles &&
			find -type f \! -executable -exec install -D -m 644 \{\} ${D}/\{\} \; &&
			find -type f -executable -exec install -D -m 755 \{\} ${D}/\{\} \; 
	)

# still need to modify php-fpm.conf with include directive
# include=/etc/fpm.d/*.conf

# sudo sed -i "s/#NAutoVTs=.*$/NAutoVTs=0/" ./target/etc/systemd/logind.conf
# sudo sed -i "s/#ReserveVT=.*$/ReserveVT=0/" ./target/etc/systemd/logind.conf
# sudo sed -i '/root/d' ./target/etc/passwd
# sudo bash -c "cat ./extraFiles/passwd >> ./target/etc/passwd"
# sudo bash -c "cat ./extraFiles/group  >> ./target/etc/group"
# #cat ./extraFiles/fstab  >> ./target/etc/fstab
# 
# sudo cp -p ./DeviceTree/zImage-Nighthawk_soc_overlay.dtb ./target/lib/firmware/.
#
	install -d ${D}${sysconfdir}/systemd/system/multi-user.target.wants/
	ln -sr ${D}/lib/systemd/system/php-fpm.service ${D}${sysconfdir}/systemd/system/multi-user.target.wants/
	ln -sr ${D}${sysconfdir}/systemd/system/instrument.service ${D}${sysconfdir}/systemd/system/multi-user.target.wants/
	ln -sr ${D}${sysconfdir}/systemd/system/launchApp.service ${D}${sysconfdir}/systemd/system/multi-user.target.wants/
}
