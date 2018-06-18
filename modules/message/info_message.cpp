#include "info_message.hpp"

InfoMessage::InfoMessage(InfoType infoType, QByteArray info)
    : infoType(infoType)
    , info(info)
{
}

InfoMessage::InfoMessage(QByteArray &infoBytes)
    : infoType(static_cast<InfoType>(infoBytes.at(0)))
    , info(infoBytes.mid(1))
{
}

InfoMessage::~InfoMessage()
{

}

Message::MessageID InfoMessage::type() const
{
    return MessageID::INFO;
}

QByteArray InfoMessage::serialize()
{
    QByteArray infoBytes;
    infoBytes.append(static_cast<char>(this->infoType));
    infoBytes.append(this->info);

    return infoBytes;
}
