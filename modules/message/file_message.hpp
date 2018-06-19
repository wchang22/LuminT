#ifndef FILE_MESSAGE_HPP
#define FILE_MESSAGE_HPP

#include <QByteArray>
#include <QFile>

namespace FileSize
{
    const int BYTE = 8;
    const int SEQ_BYTES = 2;
    const int PACKET_BYTES = 5 * pow(1024, 2);
    const uint32_t MAX_FILE_SIZE = UINT32_MAX;
}

class FileMessage
{
public:
    FileMessage(QString &filePath, uint16_t seq);
    FileMessage(QByteArray &fileBytes);

    QByteArray serialize();

    uint16_t seq;
    QFile file;
    QByteArray fileData;
};

#endif // FILE_MESSAGE_HPP
