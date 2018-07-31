#ifndef ANDROID_PERMISSIONS_HPP
#define ANDROID_PERMISSIONS_HPP

#include <QObject>
#ifdef Q_OS_ANDROID
    #include <QAndroidJniObject>
    #include <QtAndroid>
#endif

class AndroidPermissions : public QObject
{
    Q_OBJECT

public:
     void requestPermissions();

#ifdef Q_OS_ANDROID
    // Object used to obtain permissions on Android Marshmallow
    QAndroidJniObject ShowPermissionRationale;
#endif
};

#endif // ANDROID_PERMISSIONS_HPP
