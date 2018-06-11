#include "text_message.hpp"

TextMessage::TextMessage(QString &text)
{
    this->text = text;
}

TextMessage::TextMessage(QByteArray &textBytes)
{
    this->text.append(textBytes);
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
