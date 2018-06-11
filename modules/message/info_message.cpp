#include "info_message.hpp"

InfoMessage::InfoMessage(InfoType infoType, QByteArray info)
{
    this->infoType = infoType;
    this->info.append(info);
}

InfoMessage::InfoMessage(QByteArray &infoBytes)
{
    this->infoType = static_cast<InfoType>(infoBytes.at(0));
    this->info = infoBytes.mid(1);
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
