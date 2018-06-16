#ifndef TEST_COMMUNICATIONS_HPP
#define TEST_COMMUNICATIONS_HPP

#include <QtTest>

#include "modules/communications/receiver.hpp"
#include "modules/communications/sender.hpp"

class TestCommunications : public QObject
{
    Q_OBJECT

public:
    TestCommunications();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void test_connection_perfect();
    void test_connection_not_encrypted();

private:
    RegisterDeviceList registerDeviceList;
    Sender sender;
    Receiver receiver;

    QString receiverIP;
    QString receiverID;
};


#endif // TEST_COMMUNICATIONS_HPP
