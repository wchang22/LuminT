#include <QtTest>

#include "test_message.hpp"

QTEST_MAIN(TestMessage)

void TestMessage::initTestCase()
{

}

void TestMessage::cleanupTestCase()
{

}

void TestMessage::test_acknowledge_message_data()
{
    QTest::addColumn<AcknowledgeMessage::Acknowledge>("ack");

    for (int i = 0; i < (int) AcknowledgeMessage::Acknowledge::ENUM_END; i++)
        QTest::newRow(qPrintable(QString::number(i)))
                        << (AcknowledgeMessage::Acknowledge) i;
}

void TestMessage::test_acknowledge_message()
{
    QFETCH(AcknowledgeMessage::Acknowledge, ack);

    AcknowledgeMessage acknowledgeMessageFromAck(ack);
    QCOMPARE(acknowledgeMessageFromAck.ack, ack);

    QByteArray acknowledgeBytes = acknowledgeMessageFromAck.serialize();
    AcknowledgeMessage acknowledgeMessageFromBytes(acknowledgeBytes);
    QCOMPARE(acknowledgeMessageFromBytes.ack, ack);
}

void TestMessage::test_info_message_data()
{
    QList<QString> deviceKeyTestData = {"00000000", "zzzzzzzz", "12345678",
                                        "QWERTYUI", "12as34df", "123456789",
                                        ""};

    QTest::addColumn<InfoMessage::InfoType>("infoType");
    QTest::addColumn<QByteArray>("info");

    for (int i = 0; i < deviceKeyTestData.length(); i++)
        QTest::newRow(qPrintable(QString::number(i)))
                        << InfoMessage::InfoType::DEVICE_KEY
                        << deviceKeyTestData.at(0).toUtf8();
}

void TestMessage::test_info_message()
{
    QFETCH(InfoMessage::InfoType, infoType);
    QFETCH(QByteArray, info);

    InfoMessage infoMessageFromInfo(infoType, info);
    QCOMPARE(infoMessageFromInfo.infoType, infoType);
    QCOMPARE(infoMessageFromInfo.info, info);

    QByteArray infoBytes = infoMessageFromInfo.serialize();
    InfoMessage infoMessageFromBytes(infoBytes);
    QCOMPARE(infoMessageFromBytes.infoType, infoType);
    QCOMPARE(infoMessageFromBytes.info, info);
}

void TestMessage::test_request_message_data()
{
    QTest::addColumn<RequestMessage::Request>("request");

    for (int i = 0; i < (int) RequestMessage::Request::ENUM_END; i++)
        QTest::newRow(qPrintable(QString::number(i)))
                        << (RequestMessage::Request) i;
}

void TestMessage::test_request_message()
{
    QFETCH(RequestMessage::Request, request);

    RequestMessage requestMessageFromRequest(request);
    QCOMPARE(requestMessageFromRequest.request, request);

    QByteArray requestBytes = requestMessageFromRequest.serialize();
    RequestMessage requestMessageFromBytes(requestBytes);
    QCOMPARE(requestMessageFromBytes.request, request);
}

void TestMessage::test_text_message_data()
{
    QList<QString> textTestData = {"1234567890",
                                   "~`!@#$%^&*()-=_+[]\{}|;':\"<>?,./",
                                   "qwertyuiopasdfghjklzxcvbnm",
                                   "QWERTYUIOPASDFGHJKLZXCVBNM",
                                   "asdfASDF1234!@#$",
                                   "ÿ¢£ƒŒ²†‰Ø¿",
                                   "",
                                   "     ",
                                   "Hello, this a phrase",
                                   "Sed ut perspiciatis unde omnis iste natus "
                                   "error sit voluptatem accusantium "
                                   "doloremque laudantium, totam rem aperiam, "
                                   "eaque ipsa quae ab illo inventore "
                                   "veritatis et quasi architecto beatae vitae"
                                   " dicta sunt explicabo. Nemo enim ipsam "
                                   "voluptatem quia voluptas sit aspernatur "
                                   "aut odit aut fugit, sed quia consequuntur "
                                   "magni dolores eos qui ratione voluptatem "
                                   "sequi nesciunt."};

    QTest::addColumn<QString>("text");

    for (int i = 0; i < textTestData.length(); i++)
        QTest::newRow(qPrintable(QString::number(i))) << textTestData.at(i);
}

void TestMessage::test_text_message()
{
    QFETCH(QString, text);

    TextMessage textMessageFromText(text);
    QCOMPARE(textMessageFromText.text, text);

    QByteArray textBytes = textMessageFromText.serialize();
    TextMessage textMessageFromBytes(textBytes);
    QCOMPARE(textMessageFromBytes.text, text);
}
