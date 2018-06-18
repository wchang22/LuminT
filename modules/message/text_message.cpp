#include "text_message.hpp"

TextMessage::TextMessage(QString &text)
    : text(text)
{
}

TextMessage::TextMessage(QByteArray &textBytes)
    : text(textBytes)
{
}

TextMessage::~TextMessage()
{

}

Message::MessageID TextMessage::type() const
{
    return Message::MessageID::TEXT;
}

QByteArray TextMessage::serialize()
{
    return this->text.toUtf8();
}
