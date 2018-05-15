#include "info_message.hpp"

InfoMessage::InfoMessage(InfoType infoType, QVector<uint8_t> info)
{
    this->infoType = infoType;
    this->info.append(info);
}

InfoMessage::InfoMessage(QVector<uint8_t> infoVector)
{
    this->infoType = static_cast<InfoType>(infoVector.front());
    this->info = infoVector.mid(1);
}

InfoMessage::~InfoMessage()
{

}

Message::MessageID InfoMessage::type() const
{
    return MessageID::INFO;
}

QVector<uint8_t> InfoMessage::serialize()
{
    QVector<uint8_t> infoVector;
    infoVector.append(static_cast<uint8_t>(this->infoType));
    infoVector.append(this->info);

    return infoVector;
}

QVector<uint8_t> stringToByteVector(QString str)
{
    std::string stdStr = str.toStdString();
    std::vector<uint8_t> stdVec(stdStr.begin(), stdStr.end());
    QVector<uint8_t> vec = QVector<uint8_t>::fromStdVector(stdVec);

    return vec;
}

QString byteVectorToString(QVector<uint8_t> vec)
{
    std::vector<uint8_t> stdVec = vec.toStdVector();
    std::string stdStr(stdVec.begin(), stdVec.end());
    QString str = QString::fromStdString(stdStr);

    return str;
}
