# LuminT: Luminous Transfer
Wireless Data Transfer Software for Text and Files

<img src="https://raw.githubusercontent.com/wchang22/LuminT/master/images/frontPage.png" width="317" height="350" /> <img src="https://raw.githubusercontent.com/wchang22/LuminT/master/images/devicePage.png" width="317" height="350" /> 
<img src="https://raw.githubusercontent.com/wchang22/LuminT/master/images/waitingPage.png" width="317" height="350" /> <img src="https://raw.githubusercontent.com/wchang22/LuminT/master/images/receiverPage.png" width="317" height="350" />

## Requirements
* [Qt](https://www.qt.io)
* [OpenSSL](https://www.openssl.org/)

## Features
* Text and file transfer up to ~4GB on local networks
* TLS Encryption for secure data transfer
* Pause/Resume transfer capability
* Multiplatform (Windows, Linux, Android)

## Note
* Uses UASMessage from [GCOM](https://github.com/ubcuas/GCOM)

## Deploy to Windows using MinGW
qmake and windeployqt should be located in /path/to/Qt/5.xx.x/mingw*/bin/  
This will generate LuminT in the release folder. You may also need libeay32.dll and ssleay32.dll if openssl is not installed.

    $ qmake lumint.pro -spec win32-g++ -config release
    $ make qmake_all
    $ make -j4
    $ make clean
    $ windeployqt --qmldir . --release release/lumint.exe
	
	
## Deploy to Android on Windows using MinGW
Android deployment is quite a bit more work. Make sure device is connected before starting.  
Create an empty folder on your computer which will store the files before being installed on Android. This is $BUILD_TARGET.  

    $ export PROJECT_ROOT=/path/to/LuminT/src \
      export ANDROID_BIN=/path/to/Qt/5.xx.x/android_armv7/bin \
	  export PATH=$PATH:$ANDROID_BIN \
	  export JAVA_HOME=/path/to/java/jdk \
	  export ANDROID_NDK_HOST=windows-x86_64 \
      export ANDROID_NDK_ROOT=/path/to/android/ndk \
      export ANDROID_SDK_ROOT=/path/to/android/sdk \
      export ANDROID_SDK_PLATFORM=android-25 (or what is installed) \
      export ANDROID_HOME=$ANDROID_SDK_ROOT \
	  export BUILD_TARGET=/your/build/target \
	  export DEPLOY_SETTINGS=android-liblumint.so-deployment-settings.json \
	  export KEYSTORE=$PROJECT_ROOT/certificates/android_release.keystore \
    
    $ $ANDROID_BIN/qmake.exe lumint.pro -spec android-g++ -config release \
      ANDROID_EXTRA_LIBS='$PROJECT_ROOT/android/libcrypto.so $PROJECT_ROOT/android/libssl.so'
    $ make qmake_all
	$ make -j4
    $ make install INSTALL_ROOT=$BUILD_TARGET
    $ androiddeployqt --output $BUILD_TARGET --input $DEPLOY_SETTINGS --sign $KEYSTORE lumint \
      --android-platform $ANDROID_SDK_PLATFORM --reinstall (optional --device <id>)
      
For the last command, androiddeployqt will attempt to find your android device. In the case that you have multiple, you will need to add the --device <id> flag. You can find the id by running:
    
    $ adb devices -l
    
At some point, it will ask for the keystore password. The default android keystore has the password lumint-keystore
