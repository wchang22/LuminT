#-------------------------------------------------
#
# Project created by QtCreator 2018-06-17T12:47:41
#
#-------------------------------------------------

QT       += testlib network

TARGET = test_data_transfer

TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += ../../

SOURCES += \
        test_data_transfer.cpp \
        ../../modules/message/messenger.cpp \
        ../../modules/message/request_message.cpp \
        ../../modules/message/info_message.cpp \
        ../../modules/message/acknowledge_message.cpp \
        ../../modules/message/text_message.cpp \
        ../../modules/communications/receiver.cpp \
        ../../modules/communications/sender.cpp \
        ../../modules/qml/register_device_list.cpp

HEADERS += \
        test_data_transfer.hpp \
        ../../modules/message/message.hpp \
        ../../modules/message/messenger.hpp \
        ../../modules/message/request_message.hpp \
        ../../modules/message/info_message.hpp \
        ../../modules/message/acknowledge_message.hpp \
        ../../modules/message/text_message.hpp \
        ../../modules/communications/receiver.hpp \
        ../../modules/communications/sender.hpp \
        ../../modules/qml/register_device_list.hpp \

RESOURCES += \
        ../../qml.qrc
