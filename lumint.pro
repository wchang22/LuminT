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

QT += quick network
CONFIG += c++14

DEFINES += QT_DEPRECATED_WARNINGS

HEADERS += \
    modules/communications/receiver.hpp \
    modules/communications/sender.hpp \
    modules/qml/register_device_model.hpp \
    modules/qml/register_device_list.hpp \
    modules/message/message.hpp \
    modules/message/messenger.hpp \
    modules/message/request_message.hpp \
    modules/message/info_message.hpp \
    modules/message/acknowledge_message.hpp

SOURCES += main.cpp \
    modules/communications/receiver.cpp \
    modules/communications/sender.cpp \
    modules/qml/register_device_model.cpp \
    modules/qml/register_device_list.cpp \
    modules/message/messenger.cpp \
    modules/message/request_message.cpp \
    modules/message/info_message.cpp \
    modules/message/acknowledge_message.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
