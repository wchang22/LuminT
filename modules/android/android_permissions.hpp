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
    explicit AndroidPermissions(QObject *parent = nullptr);

    // Method to get the permission granted state
    bool getPermissionResult();

public slots:
     void requestPermissions();

private:

    // Variable indicating if the permission to read / write has been granted
    int permissionResult;  //  true - "Granted", false - "Denied"

#ifdef Q_OS_ANDROID
    // Object used to obtain permissions on Android Marshmallow
    QAndroidJniObject ShowPermissionRationale;
#endif
};

#endif // ANDROID_PERMISSIONS_HPP
