#include <QSignalSpy>

#include "test_communications.hpp"

const int WAIT_DURATION = 5000; // ms

QTEST_MAIN(TestCommunications)

void TestCommunications::initTestCase()
{
    // Check defaults
    QCOMPARE(sender.clientState, Sender::ClientState::DISCONNECTED);
    QCOMPARE(receiver.serverState, Receiver::ServerState::DISCONNECTED);
    QCOMPARE(sender.clientSocket.state(), QAbstractSocket::UnconnectedState);
    QCOMPARE(receiver.serverSocket, nullptr);

    // Generate config file with device key and setup sockets and IP addresses
    registerDeviceList.generateConf();
    registerDeviceList.readDeviceItems();
    sender.setup(registerDeviceList.getThisKey(), registerDeviceList);
    receiver.setup(registerDeviceList.getThisKey(), registerDeviceList);

    // Check that neither are in error state, which may occur if no internet
    QCOMPARE(sender.clientState, Sender::ClientState::DISCONNECTED);
    QCOMPARE(receiver.serverState, Receiver::ServerState::DISCONNECTED);
}

void TestCommunications::cleanupTestCase()
{

}

void TestCommunications::test_connection()
{
    /* Successful connection process should be as follows, where
     * Sender and Receiver refer to the classes and client and socket
     * refer to their respective sockets
     *
     * 1)   Sender and Receiver are initially disconnected,
     *      client is disconnected, server is null
     *
     * 2)   Sender and Receiver are setup, both have valid IP addresses,
     *      Receiver is displaying the device ID (last 1-3 digits of its IP),
     *      Sender is waiting for user to enter device ID and start connecting
     *
     * 3)   User enters device ID, Senders attempts to connect to Receiver's IP,
     *      Receiver starts server
     *
     * 4)   Sender and Receiver are now connecting, client is connecting,
     *      server is still null
     *
     * 5)   Sender is now connected, client has connected
     *      Receiver finds client's connection and accepts it
     *      Receiver is now starting encryption and server is non null and connected
     *
     * 6)   Both Sender and Receiver, client and server are encrypted
     */

    QSignalSpy senderConnectedSpy(&sender.clientSocket, &QSslSocket::connected);
    QVERIFY(senderConnectedSpy.isValid());

    // Get IP address of receiver and use it to obtain the receiver's ID
    // (Last 1-3 digits of ip address, to be entered into sender ID field)
    QString receiverIP(receiver.getIPAddress());
    QVERIFY(receiverIP.length());
    QString receiverID(receiverIP.split(QStringLiteral(".")).at(3));

    // Set ipaddress is usually done by user
    sender.setPeerIPAddress(receiverID);
    // Start connecting
    sender.connectToReceiver();
    QVERIFY(receiver.startServer());
    QCOMPARE(sender.clientState, Sender::ClientState::CONNECTING);
    QCOMPARE(receiver.serverState, Receiver::ServerState::CONNECTING);
    QCOMPARE(sender.clientSocket.state(), QAbstractSocket::ConnectingState);
    QCOMPARE(receiver.serverSocket, nullptr);

    // Check that both have connected successfully (not encrypted)
    QVERIFY(senderConnectedSpy.wait());
    QCOMPARE(senderConnectedSpy.count(), 1);

    QCOMPARE(sender.clientState, Sender::ClientState::CONNECTED);
    QCOMPARE(sender.clientSocket.state(), QAbstractSocket::ConnectedState);

    // Wait for Receiver to accept connection
    for (int i = 0; i < WAIT_DURATION && receiver.isListening(); i++)
        QTest::qWait(1);

    QVERIFY(receiver.serverSocket);
    QCOMPARE(receiver.serverSocket->state(), QAbstractSocket::ConnectedState);
    QCOMPARE(receiver.serverState, Receiver::ServerState::ENCRYPTING);

    // Wait for Sender and Receiver to be encrypted
    QSignalSpy senderEncryptedSpy(&sender.clientSocket, &QSslSocket::encrypted);
    QSignalSpy receiverEncryptedSpy(receiver.serverSocket, &QSslSocket::encrypted);
    QVERIFY(senderEncryptedSpy.isValid());
    QVERIFY(receiverEncryptedSpy.isValid());

    QVERIFY(senderEncryptedSpy.wait());
    QCOMPARE(senderEncryptedSpy.count(), 1);
    QCOMPARE(receiverEncryptedSpy.count(), 1);

    // Check both are now encrypted
    QCOMPARE(sender.clientState, Sender::ClientState::ENCRYPTED);
    QCOMPARE(receiver.serverState, Receiver::ServerState::ENCRYPTED);
    QVERIFY(sender.clientSocket.isEncrypted());
    QVERIFY(receiver.serverSocket->isEncrypted());
}


