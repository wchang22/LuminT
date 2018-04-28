#--------------------------------------------------------
#--------------------------------------------------------
# Luminous Transfer (LuminT)
# Wirelessly transfers data across devices
# Created by: Wesley Chang
# Created on: April 27, 2018
# Version: 1.0
# Version Date: April 27, 2018
#--------------------------------------------------------
#--------------------------------------------------------

QT += quick
CONFIG += c++14

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
