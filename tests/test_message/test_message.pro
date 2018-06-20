#-------------------------------------------------
#
# Project created by QtCreator 2018-06-08T20:31:49
#
#-------------------------------------------------

QT       += testlib network

TARGET = test_message

TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += ../../

SOURCES += \
        test_message.cpp \
        ../../modules/message/messenger.cpp \
        ../../modules/message/request_message.cpp \
        ../../modules/message/info_message.cpp \
        ../../modules/message/acknowledge_message.cpp \
        ../../modules/message/text_message.cpp \
        ../../modules/message/file_message.cpp \

HEADERS += \
        test_message.hpp \
        ../../modules/message/message.hpp \
        ../../modules/message/messenger.hpp \
        ../../modules/message/request_message.hpp \
        ../../modules/message/info_message.hpp \
        ../../modules/message/acknowledge_message.hpp \
        ../../modules/message/text_message.hpp \
        ../../modules/message/file_message.hpp \
