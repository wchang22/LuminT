#include <QDebug>
#include "android_permissions.hpp"

AndroidPermissions::AndroidPermissions(QObject *parent) : QObject(parent)
{

}

void AndroidPermissions::requestPermissions()
{
#ifdef Q_OS_ANDROID
    QtAndroid::PermissionResult request = QtAndroid::checkPermission("android.permission.ACCESS_NETWORK_STATE");
    qDebug() << "1";
    if (request == QtAndroid::PermissionResult::Denied)
    {
        qDebug() << "2";
        QtAndroid::requestPermissionsSync(QStringList() <<  "android.permission.ACCESS_NETWORK_STATE");
        request = QtAndroid::checkPermission("android.permission.ACCESS_NETWORK_STATE");
        qDebug() << "3";
        if (request == QtAndroid::PermissionResult::Denied)
        {
            qDebug() << "4";
            this->permissionResult = false;
            if (QtAndroid::shouldShowRequestPermissionRationale("android.permission.ACCESS_NETWORK_STATE"))
            {
                qDebug() << "5";
                ShowPermissionRationale = QAndroidJniObject("org/bytran/bytran/ShowPermissionRationale",
                                                            "(Landroid/app/Activity;)V",
                                                            QtAndroid::androidActivity().object<jobject>()
                                                            );
            }
        }
        else
        {
            this->permissionResult = true;
            qDebug() << "6";
        }
    }
    else
    {
        qDebug() << "7";
        this->permissionResult = true;
    }
#endif
}
