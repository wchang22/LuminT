#ifndef FILE_MESSAGE_HPP
#define FILE_MESSAGE_HPP

#include <QByteArray>
#include <QFile>

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
