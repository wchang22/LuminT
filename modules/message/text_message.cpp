#include "text_message.hpp"

TextMessage::TextMessage(QString &text)
{
    this->text = text;
}

TextMessage::TextMessage(QVector<uint8_t> &textVector)
{
    char* c_str = reinterpret_cast<char*>(textVector.data());
    c_str[textVector.size()] = '\0';
    this->text.append(c_str);
}

TextMessage::~TextMessage()
{

}

Message::MessageID TextMessage::type() const
{
    return Message::MessageID::TEXT;
}

QVector<uint8_t> TextMessage::serialize()
{
    std::string stdStr = this->text.toStdString();
    std::vector<uint8_t> stdVec(stdStr.begin(), stdStr.end());
    QVector<uint8_t> textVector = QVector<uint8_t>::fromStdVector(stdVec);

    return textVector;
}
