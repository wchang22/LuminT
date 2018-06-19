#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <QObject>
#include <QClipboard>

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
