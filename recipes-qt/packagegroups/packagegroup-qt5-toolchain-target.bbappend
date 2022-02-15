#qtlocation fails to compile, looks like it needs opengl?  Anyways
#we don't need it so remove.
RDEPENDS_${PN}_remove = " \
    qtlocation-dev \
    qtlocation-mkspecs \
    qtlocation-plugins \
    qtlocation-qmlplugins \
    qttranslations-qtlocation \
"
