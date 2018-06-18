#include "file_message.hpp"

FileMessage::FileMessage(QString &filePath, uint16_t seq)
    : filePath(filePath)
    , seq(seq)
    , file(filePath)
    , fileSize(file.size())

{
}

FileMessage::FileMessage(QByteArray &fileBytes)
    : seq(0)
    , fileData(fileBytes.mid(FileSize::SEQ_BYTES))
{
    for (int i = 0; i < FileSize::SEQ_BYTES; i++)
        this->seq += (fileBytes.at(i) << ((FileSize::SEQ_BYTES - i - 1) *
                                           FileSize::BYTE));
}

QByteArray FileMessage::serialize()
{
    file.open(QIODevice::ReadOnly);
    file.seek(seq * (FileSize::PACKET_BYTES - 1));
    fileData = file.read(FileSize::PACKET_BYTES - 1);
    file.close();

    return fileData;
}
