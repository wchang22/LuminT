#ifndef INFO_MESSAGE_HPP
#define INFO_MESSAGE_HPP

#include "message.hpp"

/*!
 * \brief The InfoMessage class, sends info
 */

class InfoMessage : public Message
{
public:
    enum class InfoType : char
    {
        DEVICE_KEY, // Device key of sender
        FILE_INFO,
    };

    InfoMessage(InfoType infoType, const QByteArray &info);
    InfoMessage(QByteArray &infoBytes);

    ~InfoMessage();

    Message::MessageID type() const;

    QByteArray serialize();

    InfoType infoType;
    QByteArray info;
};

#endif // INFO_MESSAGE_HPP
