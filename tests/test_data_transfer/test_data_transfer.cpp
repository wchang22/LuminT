#include "test_data_transfer.hpp"
#include "modules/utilities/utilities.hpp"

QTEST_MAIN(TestDataTransfer)

TestDataTransfer::TestDataTransfer()
    : senderRegisterDeviceList()
    , receiverRegisterDeviceList()
    , sender()
    , receiver()
    , receiverIP("")
    , receiverID("")
{
}

void TestDataTransfer::initTestCase()
{
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
