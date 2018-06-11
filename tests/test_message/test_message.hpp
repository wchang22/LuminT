#ifndef TEST_MESSAGE_HPP
#define TEST_MESSAGE_HPP

#include <QObject>
#include "modules/message/acknowledge_message.hpp"
#include "modules/message/info_message.hpp"
#include "modules/message/request_message.hpp"
#include "modules/message/text_message.hpp"
#include "modules/message/messenger.hpp"

Q_DECLARE_METATYPE(AcknowledgeMessage::Acknowledge)
Q_DECLARE_METATYPE(InfoMessage::InfoType)
Q_DECLARE_METATYPE(RequestMessage::Request)

class TestMessage : public QObject
{
    Q_OBJECT

public:

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void test_acknowledge_message_data();
    void test_acknowledge_message();

    void test_info_message_data();
    void test_info_message();

    void test_request_message_data();
    void test_request_message();

    void test_text_message_data();
    void test_text_message();
};

#endif // TEST_MESSAGE_HPP
