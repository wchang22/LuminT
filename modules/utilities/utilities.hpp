#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <QObject>
#include <QClipboard>

/*!
 * Namespace LuminT, all constants used throughout LuminT are defined here
 */
namespace LuminT
{
    const int UINT32_BYTE_REP = 4; // uint32_t is represented with 4 bytes

    const int BYTE = 8; // one byte is 8 bits
    const int KILOBYTE = 1024;
    // Message constants
    const int MESSAGE_ID_BYTES = 1;
    const int MESSAGE_SIZE_BYTES = 2;
    const int MESSAGE_CONTENT_OFFSET = MESSAGE_ID_BYTES + MESSAGE_SIZE_BYTES;
    const int MESSAGE_SIZE_INT = pow(2, MESSAGE_SIZE_BYTES * BYTE);

    // File Message constants
    const int SEQ_BYTES = 2;
    const int PACKET_BYTES = 300 * KILOBYTE;
    const uint32_t MAX_FILE_SIZE = UINT32_MAX;
    const int MAX_FILE_SIZE_REP = UINT32_BYTE_REP;

    // Device Key constants
    const QString CONFIG_FILE_NAME("lumint.conf");
    const int DEVICE_KEY_SIZE = 8;

    // Communications constants
    const int PORT = 4002;
    const int ENCRYPTING_TIMEOUT = 5000; // ms
}

class Utilities : public QObject
{
    Q_OBJECT
public:
    Utilities(QClipboard *clipboard);

    static QByteArray uint32ToByteArray(uint32_t integer);
    static uint32_t byteArrayToUint32(QByteArray byteArray);

public slots:
    QString paste() const;
    void copy(QString text);

private:
    QClipboard *clipboard;

};

#endif // UTILITIES_HPP
