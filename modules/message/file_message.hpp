#ifndef FILE_MESSAGE_HPP
#define FILE_MESSAGE_HPP

#include <QByteArray>
#include <QFile>

namespace FileSize
{
    const int BYTE = 8;
    const int SEQ_BYTES = 2;
    const int PACKET_BYTES = 5 * pow(1024, 2);
}

class FileMessage
{
public:
    FileMessage(QString &filePath, uint16_t seq);
    FileMessage(QByteArray &fileBytes);

    QByteArray serialize();

    QString filePath;
    uint16_t seq;
    QFile file;
    uint64_t fileSize;
    QByteArray fileData;
};

#endif // FILE_MESSAGE_HPP
