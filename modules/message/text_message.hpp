#ifndef TEXT_MESSAGE_HPP
#define TEXT_MESSAGE_HPP

#include "message.hpp"

class TextMessage : public Message
{
public:
    TextMessage(QString &text);
    TextMessage(QVector<uint8_t> &textVector);

    ~TextMessage();

    Message::MessageID type() const;

    QVector<uint8_t> serialize();

    QString text;
};

#endif // TEXT_MESSAGE_HPP
