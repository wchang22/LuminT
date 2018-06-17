#ifndef TEST_DATA_TRANSFER_HPP
#define TEST_DATA_TRANSFER_HPP

#include <QtTest>

#include "modules/communications/receiver.hpp"
#include "modules/communications/sender.hpp"

class TestDataTransfer : public QObject
{
    Q_OBJECT

public:
    TestDataTransfer();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void test_send_text_data();
    void test_send_text();

private:
    RegisterDeviceList senderRegisterDeviceList;
    RegisterDeviceList receiverRegisterDeviceList;
    Sender sender;
    Receiver receiver;

    QString receiverIP;
    QString receiverID;
};


#endif // TEST_DATA_TRANSFER_HPP
