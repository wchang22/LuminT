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

# Unit/Integration testing
test {
    TEMPLATE = subdirs
    SUBDIRS += \
               tests/test_message \
               tests/test_communications \
               tests/test_data_transfer \
}

HEADERS += \
    modules/communications/receiver.hpp \
    modules/communications/sender.hpp \
    modules/qml/register_device_model.hpp \
    modules/qml/register_device_list.hpp \
    modules/message/message.hpp \
    modules/message/messenger.hpp \
    modules/message/request_message.hpp \
    modules/message/info_message.hpp \
    modules/message/acknowledge_message.hpp \
    modules/message/text_message.hpp \
    modules/utilities/utilities.hpp \
    modules/message/file_message.hpp


SOURCES += main.cpp \
    modules/communications/receiver.cpp \
    modules/communications/sender.cpp \
    modules/qml/register_device_model.cpp \
    modules/qml/register_device_list.cpp \
    modules/message/messenger.cpp \
    modules/message/request_message.cpp \
    modules/message/info_message.cpp \
    modules/message/acknowledge_message.cpp \
    modules/message/text_message.cpp \
    modules/utilities/utilities.cpp \
    modules/message/file_message.cpp

RESOURCES += qml.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32 {
    LIBS += $$PWD/openssl/libeay32.lib \
            $$PWD/openssl/ssleay32.lib
}

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    QT += androidextras

    HEADERS += modules/android/android_permissions.hpp
    SOURCES += modules/android/android_permissions.cpp

    ANDROID_EXTRA_LIBS = \
        $$PWD/android/libcrypto.so \
        $$PWD/android/libssl.so
}

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
