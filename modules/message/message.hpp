#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <QVector>

class Message
{
public:

    enum class MessageID : int
    {
        INVALID         = -1,
        INFO            = 1,
        REQUEST         = 2,
        ACKNOWLEDGE     = 3,
    };

    virtual ~Message() {}

    virtual MessageID type() const = 0;

    virtual QVector<uint8_t> serialize() = 0;
};

#endif // MESSAGE_HPP
