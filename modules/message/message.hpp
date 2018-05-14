#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <QByteArray>

class Message
{
public:

    enum class MessageID : uint8_t
    {
        SEND            = 0x01,
        REQUEST         = 0x02,
        ACKNOWLEDGE     = 0x03,
    };

    virtual ~Message() {}

    virtual MessageID type() = 0;

    virtual QByteArray serialize() = 0;
};

#endif // MESSAGE_HPP
