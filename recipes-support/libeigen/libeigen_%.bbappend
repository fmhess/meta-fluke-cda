#hack so we can pull the license package into the image using RDEPEND since openembedded doesn't automatically RPROVIDE the license packages it generates
RPROVIDES_${PN} += "${PN}-lic"
