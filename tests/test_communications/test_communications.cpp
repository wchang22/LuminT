#include <QSignalSpy>
#include <QLoggingCategory>

#include "test_communications.hpp"
#include "modules/utilities/utilities.hpp"

const int WAIT_DURATION = 5000; // ms
const int PORT = 4002;

QTEST_MAIN(TestCommunications)

TestCommunications::TestCommunications()
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

void TestCommunications::initTestCase()
{
    // Check defaults
    QCOMPARE(sender.clientState, Sender::ClientState::DISCONNECTED);
    QCOMPARE(receiver.serverState, Receiver::ServerState::DISCONNECTED);
    QCOMPARE(sender.clientSocket.state(), QAbstractSocket::UnconnectedState);
    QCOMPARE(receiver.serverSocket, nullptr);

    // Generate config file with device key and setup sockets and IP addresses

    senderRegisterDeviceList.generateConf();
    senderRegisterDeviceList.readDeviceItems();
    QVERIFY(QFile::remove(LuminT::CONFIG_FILE_NAME));
    receiverRegisterDeviceList.generateConf();
    receiverRegisterDeviceList.readDeviceItems();
    sender.setup(senderRegisterDeviceList);
    receiver.setup(receiverRegisterDeviceList);

    // Check that neither are in error state, which may occur if no internet
    QCOMPARE(sender.clientState, Sender::ClientState::DISCONNECTED);
    QCOMPARE(receiver.serverState, Receiver::ServerState::DISCONNECTED);
}

void TestCommunications::cleanupTestCase()
{

}

void TestCommunications::test_connection_perfect()
{
    /*
     * Successful connection process should be as follows, where
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
    receiverIP = receiver.getIPAddress();
    QVERIFY(receiverIP.length());
    receiverID = receiverIP.split(QStringLiteral(".")).at(3);

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

    // Cut connection for next test
    QSignalSpy receiverDisconnectedSpy(&receiver, &Receiver::disconnected);
    QVERIFY(receiverDisconnectedSpy.isValid());

    sender.clientSocket.abort();
    QVERIFY(receiverDisconnectedSpy.wait());
    QCOMPARE(receiverDisconnectedSpy.count(), 1);

    // Check everything is now disconnected
    QCOMPARE(sender.clientState, Sender::ClientState::DISCONNECTED);
    QCOMPARE(receiver.serverState, Receiver::ServerState::DISCONNECTED);
    QCOMPARE(sender.clientSocket.state(), QAbstractSocket::UnconnectedState);
    QCOMPARE(receiver.serverSocket->state(), QAbstractSocket::UnconnectedState);
}

void TestCommunications::test_connection_not_encrypted()
{
    /*
     * Occurs when TCP connection succeeds but TLS handshake fails
     * May occur if device accidentally connects to a non-LuminT program
     * or if OpenSSL is not supported
     *
     * Connection should timeout and cut after 5s
     */

    // Test connecting a fake client to Receiver, should connect but never encrypt
    QTcpSocket client(this);

    QSignalSpy clientConnectedSpy(&client, &QTcpSocket::connected);
    QVERIFY(clientConnectedSpy.isValid());

    // Start connecting
    client.connectToHost(QHostAddress(receiverIP), PORT);
    QVERIFY(receiver.startServer());

    // Check that both have connected successfully (not encrypted)
    QVERIFY(clientConnectedSpy.wait());
    QCOMPARE(clientConnectedSpy.count(), 1);

    QCOMPARE(client.state(), QAbstractSocket::ConnectedState);

    // Wait for Receiver to accept connection
    for (int i = 0; i < WAIT_DURATION && receiver.isListening(); i++)
        QTest::qWait(1);

    QVERIFY(receiver.serverSocket);
    QCOMPARE(receiver.serverSocket->state(), QAbstractSocket::ConnectedState);
    QCOMPARE(receiver.serverState, Receiver::ServerState::ENCRYPTING);

    // Check that receiver never encrypts
    QSignalSpy receiverEncryptedSpy(receiver.serverSocket, &QSslSocket::encrypted);
    QVERIFY(receiverEncryptedSpy.isValid());

    QVERIFY(!receiverEncryptedSpy.wait(1000));
    QCOMPARE(receiverEncryptedSpy.count(), 0);

    // Check that waiting for encryption times out
    QSignalSpy receiverConnectionErrorSpy(&receiver, &Receiver::connectionError);
    QVERIFY(receiverConnectionErrorSpy.isValid());

    QVERIFY(receiverConnectionErrorSpy.wait());
    QCOMPARE(receiverConnectionErrorSpy.count(), 1);
    QCOMPARE(receiver.serverSocket->state(), QAbstractSocket::UnconnectedState);
    QCOMPARE(receiver.serverState, Receiver::ServerState::DISCONNECTED);


    // Repeat the process now with a fake server
    QTcpServer server(this);

    QSignalSpy senderConnectedSpy(&sender.clientSocket, &QSslSocket::connected);
    QVERIFY(senderConnectedSpy.isValid());

    // Start connecting
    sender.connectToReceiver();
    QVERIFY(server.listen(QHostAddress(receiverIP), PORT));

    // Check that sender has connected successfully (not encrypted)
    QVERIFY(senderConnectedSpy.wait());
    QCOMPARE(senderConnectedSpy.count(), 1);

    QCOMPARE(sender.clientSocket.state(), QAbstractSocket::ConnectedState);
    QCOMPARE(sender.clientState, Sender::ClientState::CONNECTED);

    // Check that sender never encrypts
    QSignalSpy senderEncryptedSpy(&sender.clientSocket, &QSslSocket::encrypted);
    QVERIFY(senderEncryptedSpy.isValid());

    QVERIFY(!senderEncryptedSpy.wait(1000));
    QCOMPARE(senderEncryptedSpy.count(), 0);

    // Check that waiting for encryption times out
    QSignalSpy senderConnectionErrorSpy(&sender, &Sender::connectionError);
    QVERIFY(senderConnectionErrorSpy.isValid());

    QVERIFY(senderConnectionErrorSpy.wait());
    QCOMPARE(senderConnectionErrorSpy.count(), 1);
    QCOMPARE(sender.clientSocket.state(), QAbstractSocket::UnconnectedState);
    QCOMPARE(sender.clientState, Sender::ClientState::DISCONNECTED);
}

void TestCommunications::test_key_verification()
{
    /*
     * After successful encryption, key verification should follow:
     *
     * 1) Receiver sends a request for Sender's key
     * 2) Sender sends its key in an info message
     * 3) If Receiver has Sender's key in its list of recognized keys,
     *      Receiver sends its own key to Sender in an info message
     *    If Receiver does not, it sends an error acknowledge message
     * 4) If Sender receives Receiver's key, it checks with its list
     *      If Sender recognizes it, it sends a device key ok acknowledgement
     *          and is now ready
     *      If Sender does not recognize it, it sends an error acknowledge
     *    If Sender receives an acknowledge error message, it cuts the connection
     * 5) If Receiver receives a device key ok acknowledgement, it is now ready
     *    If Receiver receives an error acknowledge, it cuts the connection
     */

    // Connect sender and receiver and make sure both are encrypted
    sender.connectToReceiver();
    QVERIFY(receiver.startServer());

    // If neither keys are registered, connection should be cut
    QSignalSpy receiverConnectionUnrecognizedSpy(
                    &receiver, &Receiver::connectionUnrecognized);
    QSignalSpy senderConnectionUnrecognizedSpy(
                    &sender, &Sender::connectionUnrecognized);
    QVERIFY(receiverConnectionUnrecognizedSpy.isValid());
    QVERIFY(senderConnectionUnrecognizedSpy.isValid());

    // Receiver sends request, Sender sends key, Receiver sends ack error
    // Sender cuts connection, Receiver is the second to emit unrecognized
    QVERIFY(receiverConnectionUnrecognizedSpy.wait());
    QCOMPARE(receiverConnectionUnrecognizedSpy.count(), 1);
    QCOMPARE(senderConnectionUnrecognizedSpy.count(), 1);
    receiverConnectionUnrecognizedSpy.clear();
    senderConnectionUnrecognizedSpy.clear();
    QCOMPARE(sender.clientState, Sender::ClientState::DISCONNECTED);
    QCOMPARE(receiver.serverState, Receiver::ServerState::DISCONNECTED);
    QCOMPARE(sender.clientSocket.state(), QAbstractSocket::UnconnectedState);
    QCOMPARE(receiver.serverSocket->state(), QAbstractSocket::UnconnectedState);

    // If one key is registered, connection should also be cut
    senderRegisterDeviceList.deviceItems.append({
         receiverRegisterDeviceList.getThisKey(), true,
         1, QStringLiteral("\u2013") });

    sender.connectToReceiver();
    QVERIFY(receiver.startServer());

    // Receiver sends request, Sender sends key, Receiver sends ack error
    // Sender cuts connection, Receiver is the second to emit unrecognized
    QVERIFY(receiverConnectionUnrecognizedSpy.wait());
    QCOMPARE(receiverConnectionUnrecognizedSpy.count(), 1);
    QCOMPARE(senderConnectionUnrecognizedSpy.count(), 1);
    receiverConnectionUnrecognizedSpy.clear();
    senderConnectionUnrecognizedSpy.clear();
    QCOMPARE(sender.clientState, Sender::ClientState::DISCONNECTED);
    QCOMPARE(receiver.serverState, Receiver::ServerState::DISCONNECTED);
    QCOMPARE(sender.clientSocket.state(), QAbstractSocket::UnconnectedState);
    QCOMPARE(receiver.serverSocket->state(), QAbstractSocket::UnconnectedState);

    // Same, but with the other key registered
    senderRegisterDeviceList.deviceItems.removeLast();
    receiverRegisterDeviceList.deviceItems.append({
         senderRegisterDeviceList.getThisKey(), true,
         1, QStringLiteral("\u2013") });

    sender.connectToReceiver();
    QVERIFY(receiver.startServer());

    // Receiver sends request, Sender sends key, Receiver sends key,
    // Sender sends ack error
    // Receiver cuts connection, Sender is the second to emit unrecognized
    QVERIFY(senderConnectionUnrecognizedSpy.wait());
    QCOMPARE(receiverConnectionUnrecognizedSpy.count(), 1);
    QCOMPARE(senderConnectionUnrecognizedSpy.count(), 1);
    receiverConnectionUnrecognizedSpy.clear();
    senderConnectionUnrecognizedSpy.clear();
    QCOMPARE(sender.clientState, Sender::ClientState::DISCONNECTED);
    QCOMPARE(receiver.serverState, Receiver::ServerState::DISCONNECTED);
    QCOMPARE(sender.clientSocket.state(), QAbstractSocket::UnconnectedState);
    QCOMPARE(receiver.serverSocket->state(), QAbstractSocket::UnconnectedState);

    // Connection should succeed if both are registered
    QSignalSpy senderConnectedSpy(&sender, &Sender::connected);
    QSignalSpy receiverConnectedSpy(&receiver, &Receiver::connected);
    QVERIFY(senderConnectedSpy.isValid());
    QVERIFY(receiverConnectedSpy.isValid());

    senderRegisterDeviceList.deviceItems.append({
         receiverRegisterDeviceList.getThisKey(), true,
         1, QStringLiteral("\u2013") });

    sender.connectToReceiver();
    QVERIFY(receiver.startServer());

    // Receiver sends request, Sender sends key, Receiver sends key,
    // Sender sends device key ok ack, Sender is now ready
    // Receiver receives ack, Receiver is the second to emit connected
    QVERIFY(receiverConnectedSpy.wait());
    QCOMPARE(senderConnectedSpy.count(), 1);
    QCOMPARE(receiverConnectedSpy.count(), 1);
    QCOMPARE(sender.clientState, Sender::ClientState::ENCRYPTED);
    QCOMPARE(receiver.serverState, Receiver::ServerState::ENCRYPTED);
    QCOMPARE(sender.clientSocket.state(), QAbstractSocket::ConnectedState);
    QCOMPARE(receiver.serverSocket->state(), QAbstractSocket::ConnectedState);
}


