#include <QLoggingCategory>

#include "test_data_transfer.hpp"
#include "modules/utilities/utilities.hpp"

const QString PROJECT_ROOT(QString(CWD).section("/", 0, -3));

QTEST_MAIN(TestDataTransfer)

TestDataTransfer::TestDataTransfer()
    : senderRegisterDeviceList()
    , receiverRegisterDeviceList()
    , sender()
    , receiver()
    , receiverIP("")
    , receiverID("")
{
    // Disable Ssl warnings
    QLoggingCategory::setFilterRules("qt.network.ssl.warning=false");
}

void TestDataTransfer::initTestCase()
{
    qRegisterMetaType<std::shared_ptr<InfoMessage>>();
    qRegisterMetaType<std::shared_ptr<RequestMessage>>();

    // Setup devices, IP address, Keys
    senderRegisterDeviceList.generateConf();
    senderRegisterDeviceList.readDeviceItems();
    QVERIFY(QFile::remove(LuminT::CONFIG_FILE_NAME));
    receiverRegisterDeviceList.generateConf();
    receiverRegisterDeviceList.readDeviceItems();

    sender.setup(senderRegisterDeviceList);
    receiver.setup(receiverRegisterDeviceList);

    // Get receiverID
    receiverIP = receiver.getIPAddress();
    QVERIFY(receiverIP.length());
    receiverID = receiverIP.split(QStringLiteral(".")).at(3);

    // Register each other's device keys
    senderRegisterDeviceList.deviceItems.append({
         receiverRegisterDeviceList.getThisKey(), true,
         1, QStringLiteral("\u2013") });

    receiverRegisterDeviceList.deviceItems.append({
         senderRegisterDeviceList.getThisKey(), true,
         1, QStringLiteral("\u2013") });

    // Connect Sender and Receiver
    QSignalSpy senderConnectedSpy(&sender, &Sender::connected);
    QSignalSpy receiverConnectedSpy(&receiver, &Receiver::connected);
    QVERIFY(senderConnectedSpy.isValid());
    QVERIFY(receiverConnectedSpy.isValid());

    sender.setPeerIPAddress(receiverID);
    sender.connectToReceiver();
    QVERIFY(receiver.startServer());

    QVERIFY(receiverConnectedSpy.wait());
    QCOMPARE(receiverConnectedSpy.count(), 1);
    QCOMPARE(senderConnectedSpy.count(), 1);
}

void TestDataTransfer::cleanupTestCase()
{

}

void TestDataTransfer::test_send_text_data()
{
    QList<QString> textTestData = {"Hello",
                                   "This is test data",
                                   "1234567890qwertyuiopASDFGHJKL",
                                   "!@#$%^&*()_~{}:\"<>?",
                                   "♠←™§²Õ"};

    QTest::addColumn<QString>("text");

    for (int i = 0; i < textTestData.length(); i++)
        QTest::newRow(qPrintable(QString::number(i))) << textTestData[i];
}

void TestDataTransfer::test_send_text()
{
    QFETCH(QString, text);

    QSignalSpy receivedTextSpy(&receiver, &Receiver::receivedText);

    sender.sendTextMessage(text);
    QVERIFY(receivedTextSpy.wait());
    QCOMPARE(receivedTextSpy.count(), 1);

    QList<QVariant> textList = receivedTextSpy.takeFirst();
    QCOMPARE(textList[0].toString(), text);
}

void TestDataTransfer::test_send_file_data()
{
    QList<QString> filePathTestData = {
        PROJECT_ROOT + QStringLiteral("/openssl/libeay32.lib"),
        PROJECT_ROOT + QStringLiteral("/certificates/rootCA.pem"),
        // Uncomment to test fully, though this file is quite large
        // Replace with path to a large file, eg. 200 KB
        // QStringLiteral("C:/Qt/5.11.0/mingw53_32/Qt5Guid.dll")
    };

    QTest::addColumn<QString>("filePath");

    for (int i = 0; i < filePathTestData.length(); i++)
        QTest::newRow(qPrintable(QString::number(i))) << filePathTestData[i];
}

void TestDataTransfer::test_send_file()
{
    QFETCH(QString, filePath);

    // Set receive destination and send file
    receiver.setFilePath(CWD);
    QVERIFY(sender.sendFile(filePath));

    // Receiver receives file info - file size and file name
    QSignalSpy infoSpy(&receiver, &Receiver::receivedInfo);
    QVERIFY(infoSpy.isValid());
    QVERIFY(infoSpy.wait());
    QCOMPARE(infoSpy.count(), 1);

    // This is usually called from qml
    receiver.requestFirstPacket();

    // Sender receives first file packet request
    QSignalSpy requestSpy(&sender, &Sender::receivedRequest);
    QVERIFY(requestSpy.isValid());
    QVERIFY(requestSpy.wait());
    QCOMPARE(requestSpy.count(), 1);

    // Wait up to 60s for all files to transfer
    QSignalSpy receiverFileCompletedSpy(&receiver, &Receiver::fileCompleted);
    QVERIFY(receiverFileCompletedSpy.isValid());
    QVERIFY(receiverFileCompletedSpy.wait(60000));
    QCOMPARE(receiverFileCompletedSpy.count(), 1);

    QSignalSpy senderFileCompletedSpy(&sender, &Sender::fileCompleted);
    QVERIFY(senderFileCompletedSpy.isValid());
    QVERIFY(senderFileCompletedSpy.wait());
    QCOMPARE(senderFileCompletedSpy.count(), 1);

    // Check both files match
    QString targetFilePath(CWD + QStringLiteral("/") +
                           filePath.section("/", -1, -1));
    QFile actualFile(targetFilePath);
    QFile expectedFile(filePath);

    QVERIFY(expectedFile.open(QIODevice::ReadOnly));
    QVERIFY(actualFile.open(QIODevice::ReadOnly));

    QCOMPARE(actualFile.readAll(), expectedFile.readAll());

    expectedFile.close();
    actualFile.close();

    // Remove file when done
    QVERIFY(actualFile.remove());
}

void TestDataTransfer::test_cancel_file()
{
    QString filePath(QStringLiteral("C:/Qt/5.11.0/mingw53_32/Qt5Guid.dll"));
    QFile file(CWD + QStringLiteral("/") + filePath.section("/", -1, -1));

    QVERIFY(sender.sendFile(filePath));

    QSignalSpy infoSpy(&receiver, &Receiver::receivedInfo);
    QVERIFY(infoSpy.isValid());
    QVERIFY(infoSpy.wait());
    QCOMPARE(infoSpy.count(), 1);

    receiver.requestFirstPacket();

    QSignalSpy requestSpy(&sender, &Sender::receivedRequest);
    QVERIFY(requestSpy.isValid());
    QVERIFY(requestSpy.wait());
    QCOMPARE(requestSpy.count(), 1);

    // Wait a bit for a bit of the file to transfer
    QTest::qWait(100);
    receiver.cancelFileTransfer();

    QSignalSpy senderFileCompletedSpy(&sender, &Sender::fileCompleted);
    QVERIFY(senderFileCompletedSpy.isValid());
    QVERIFY(senderFileCompletedSpy.wait());
    QCOMPARE(senderFileCompletedSpy.count(), 1);

    // Make sure file has been deleted
    QVERIFY(!file.exists());
}

void TestDataTransfer::test_pause_file()
{
    QString filePath(PROJECT_ROOT + QStringLiteral("/openssl/libeay32.lib"));

    QVERIFY(sender.sendFile(filePath));

    QSignalSpy infoSpy(&receiver, &Receiver::receivedInfo);
    QVERIFY(infoSpy.isValid());
    QVERIFY(infoSpy.wait());
    QCOMPARE(infoSpy.count(), 1);

    receiver.requestFirstPacket();

    QSignalSpy requestSpy(&sender, &Sender::receivedRequest);
    QVERIFY(requestSpy.isValid());
    QVERIFY(requestSpy.wait());
    QCOMPARE(requestSpy.count(), 1);

    // Wait a bit for a bit of the file to transfer
    QTest::qWait(100);
    receiver.pauseFileTransfer();

    QSignalSpy filePausedSpy(&receiver, &Receiver::filePaused);
    QVERIFY(filePausedSpy.isValid());
    QVERIFY(filePausedSpy.wait());
    QCOMPARE(filePausedSpy.count(), 1);

    // File pause request
    requestSpy.clear();
    QVERIFY(requestSpy.wait());
    QCOMPARE(requestSpy.count(), 1);

    receiver.resumeFileTransfer();

    // Next packet request
    requestSpy.clear();
    QVERIFY(requestSpy.wait());
    QCOMPARE(requestSpy.count(), 1);

    QSignalSpy receiverFileCompletedSpy(&receiver, &Receiver::fileCompleted);
    QVERIFY(receiverFileCompletedSpy.isValid());
    QVERIFY(receiverFileCompletedSpy.wait());
    QCOMPARE(receiverFileCompletedSpy.count(), 1);

    QSignalSpy senderFileCompletedSpy(&sender, &Sender::fileCompleted);
    QVERIFY(senderFileCompletedSpy.isValid());
    QVERIFY(senderFileCompletedSpy.wait());
    QCOMPARE(senderFileCompletedSpy.count(), 1);

    // Check both files match
    QString targetFilePath(CWD + QStringLiteral("/") +
                           filePath.section("/", -1, -1));
    QFile actualFile(targetFilePath);
    QFile expectedFile(filePath);

    QVERIFY(expectedFile.open(QIODevice::ReadOnly));
    QVERIFY(actualFile.open(QIODevice::ReadOnly));

    QCOMPARE(actualFile.readAll(), expectedFile.readAll());

    expectedFile.close();
    actualFile.close();

    // Remove file when done
    QVERIFY(actualFile.remove());
}
