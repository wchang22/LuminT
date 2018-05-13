#ifndef REGISTER_DEVICE_LIST_HPP
#define REGISTER_DEVICE_LIST_HPP

#include <QObject>
#include <QVector>
#include <QFile>

const QString CONFIG_FILE_NAME = "lumint.conf";
const int DEVICE_ID_SIZE = 8;

struct RegisterDeviceItem
{
    QString deviceID;
    bool readOnlyStatus;
    int seq;
    QString buttonText;

};

class RegisterDeviceList : public QObject
{
    Q_OBJECT
public:
    RegisterDeviceList();
    ~RegisterDeviceList();

    QVector<RegisterDeviceItem> items() const;

    bool setItemAt(int index, const RegisterDeviceItem &item);

signals:
    void preItemInserted();
    void postItemInserted();

    void preItemRemoved(int index);
    void postItemRemoved();

public slots:
    void insertItem();
    void removeItem(int index);
    bool checkNewDeviceID();

    void readDeviceItems();
    void writeDeviceItems();

private:
    QVector<RegisterDeviceItem> deviceItems;
    QFile configFile;
};

#endif // REGISTER_DEVICE_LIST_HPP
