#include "android_permissions.hpp"

#ifdef Q_OS_ANDROID
using namespace QtAndroid;
#endif

void AndroidPermissions::requestPermissions()
{
#ifdef Q_OS_ANDROID
    PermissionResult request = checkPermission("android.permission.READ_EXTERNAL_STORAGE");

    if (request == PermissionResult::Granted)
        return;

    requestPermissionsSync(QStringList() <<  "android.permission.READ_EXTERNAL_STORAGE");
    request = checkPermission("android.permission.READ_EXTERNAL_STORAGE");

    if (request == PermissionResult::Granted)
        return;

    if (!shouldShowRequestPermissionRationale("android.permission.READ_EXTERNAL_STORAGE"))
        return;

    // TODO: show permission rationale using Java
    ShowPermissionRationale = QAndroidJniObject("org/bytran/bytran/ShowPermissionRationale",
                                                "(Landroid/app/Activity;)V",
                                                androidActivity().object<jobject>());
#endif
}
