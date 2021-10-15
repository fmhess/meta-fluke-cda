# FlukeCDA: we remove qemu because the Zeus version of
# nativesdk-qemu (4.1) breaks
# with the "-fcommon" gcc flag (which is needed to compile Zeus with gcc 10).
# Once we move to something newer than Zeus, the problem should go away
# with newer versions of qemu, and this file will no longer be needed.

# Also, libxcb (a dependency of qemu) breaks with newer versions of Python.
# This problem should presumably also go away with newer versions of libxcb.

RDEPENDS_${PN}_remove = " \
    nativesdk-qemu \
    nativesdk-qemu-helper \
"
