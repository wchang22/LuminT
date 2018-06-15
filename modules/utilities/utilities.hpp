#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <QObject>
#include <QClipboard>

class Utilities : public QObject
{
    Q_OBJECT
public:
    Utilities(QClipboard *clipboard);

public slots:
    QString paste() const;
    void copy(QString text);

private:
    QClipboard *clipboard;

};

#endif // UTILITIES_HPP
