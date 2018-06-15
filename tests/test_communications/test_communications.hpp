#ifndef TEST_COMMUNICATIONS_HPP
#define TEST_COMMUNICATIONS_HPP

#include <QtTest>

#include "modules/communications/receiver.hpp"
#include "modules/communications/sender.hpp"

class TestCommunications : public QObject
{
    Q_OBJECT

public:

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void test_connection();

private:
    RegisterDeviceList registerDeviceList;
    Sender sender;
    Receiver receiver;
};


#endif // TEST_COMMUNICATIONS_HPP
