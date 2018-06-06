#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <QVector>

class Message
{
public:

    enum class MessageID : uint8_t
    {
        INVALID         = 0,
        INFO            = 1,
        REQUEST         = 2,
        ACKNOWLEDGE     = 3,
        TEXT            = 4,
    };

    virtual ~Message() {}

    virtual MessageID type() const = 0;

    virtual QVector<uint8_t> serialize() = 0;
};

#endif // MESSAGE_HPP
