#include <QSignalSpy>

#include "test_communications.hpp"

QTEST_MAIN(TestCommunications)

void TestCommunications::initTestCase()
{
    registerDeviceList.generateConf();
    registerDeviceList.readDeviceItems();
    sender.setup(registerDeviceList.getThisKey(), registerDeviceList);
    receiver.setup(registerDeviceList.getThisKey(), registerDeviceList);
}

void TestCommunications::cleanupTestCase()
{

}

void TestCommunications::test_connection()
{
    QSignalSpy connectedSpy(&sender.clientSocket, &QAbstractSocket::connected);
    QVERIFY(connectedSpy.isValid());

    sender.setPeerIPAddress(receiver.getIPAddress());
    sender.connectToReceiver();
    QVERIFY(receiver.startServer());
    QVERIFY(connectedSpy.wait());
    QCOMPARE(connectedSpy.count(), 1);

}


