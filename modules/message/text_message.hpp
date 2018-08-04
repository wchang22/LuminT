#ifndef TEXT_MESSAGE_HPP
#define TEXT_MESSAGE_HPP

#include <QString>

#include "message.hpp"

class TextMessage : public Message
{
public:
    TextMessage(const QString &text);
    TextMessage(QByteArray &textVector);

    ~TextMessage();

    Message::MessageID type() const;

    QByteArray serialize();

    QString text;
};

#endif // TEXT_MESSAGE_HPP
