#include "file_message.hpp"
#include "modules/utilities/utilities.hpp"

FileMessage::FileMessage(QString &filePath, uint16_t seq)
    : seq(seq)
    , file(filePath)

{
}

FileMessage::FileMessage(QByteArray &fileBytes)
    : seq(0)
    , fileData(fileBytes.mid(LuminT::SEQ_BYTES))
{
    for (int i = 0; i < LuminT::SEQ_BYTES; i++)
        this->seq += (fileBytes.at(i) << ((LuminT::SEQ_BYTES - i - 1) * LuminT::BYTE));
}

QByteArray FileMessage::serialize()
{
    file.open(QIODevice::ReadOnly);
    file.seek(seq * (LuminT::PACKET_BYTES));
    fileData = file.read(LuminT::PACKET_BYTES);
    file.close();

    return fileData;
}
