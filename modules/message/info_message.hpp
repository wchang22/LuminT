#ifndef INFO_MESSAGE_HPP
#define INFO_MESSAGE_HPP

#include "message.hpp"

class InfoMessage : public Message
{
public:
    enum class InfoType : int
    {
        DEVICE_ID       = 1,
        MESSAGE_TYPE    = 2,
        MESSAGE_LENGTH  = 3,
    };

    InfoMessage(InfoType infoType, QVector<uint8_t> info);
    InfoMessage(QVector<uint8_t> infoVector);

    ~InfoMessage();

    Message::MessageID type() const;

    QVector<uint8_t> serialize();

    InfoType infoType;
    QVector<uint8_t> info;
};

QVector<uint8_t> stringToByteVector(QString str);

QString byteVectorToString(QVector<uint8_t> vec);

#endif // INFO_MESSAGE_HPP
