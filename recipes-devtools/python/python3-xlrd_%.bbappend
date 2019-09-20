# add support for building nativesdk-python2-xlrd by setting BBLASSEXTEND, which
# the python-xlrd.inc from openembedded currently fails to do
BBCLASSEXTEND = "native nativesdk"
