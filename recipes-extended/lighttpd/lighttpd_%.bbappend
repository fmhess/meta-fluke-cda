
fluke_common_do_install() {
	rm -rf ${D}/www/
}

do_install_append_fluke-cda-nighthawk() {
	fluke_common_do_install
}
do_install_append_fluke-cda-caldera() {
	fluke_common_do_install
}
