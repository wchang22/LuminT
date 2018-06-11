#ifndef INFO_MESSAGE_HPP
#define INFO_MESSAGE_HPP

#include "message.hpp"

/*!
 * \brief The InfoMessage class, sends info
 */

class InfoMessage : public Message
{
public:
    enum class InfoType : uint8_t
    {
        DEVICE_KEY, // Device key of sender
    };

    InfoMessage(InfoType infoType, QByteArray info);
    InfoMessage(QByteArray &infoBytes);

    ~InfoMessage();

    Message::MessageID type() const;

    QByteArray serialize();

    InfoType infoType;
    QByteArray info;
};

#endif // INFO_MESSAGE_HPP
