#ifndef REGISTER_DEVICE_LIST_HPP
#define REGISTER_DEVICE_LIST_HPP

#include <QObject>
#include <QVector>
#include <QFile>

const QString CONFIG_FILE_NAME = "lumint.conf";
const int DEVICE_KEY_SIZE = 8;

struct RegisterDeviceItem
{
    QString deviceKey;
    bool readOnlyStatus;
    int seq;
    QString buttonText;
};

class RegisterDeviceList : public QObject
{
    Q_OBJECT

    friend class TestCommunications;
    friend class TestDataTransfer;

public:
    RegisterDeviceList();
    ~RegisterDeviceList();

    QVector<RegisterDeviceItem> items() const;

    bool setItemAt(int index, const RegisterDeviceItem &item);
    void generateConf();

signals:
    void preItemInserted(int index);
    void postItemInserted();

    void preItemRemoved(int index);
    void postItemRemoved();

public slots:
    void insertItem();
    void removeItem(int index);
    bool checkNewDeviceKey();
    QString getThisKey() const;

    void readDeviceItems();
    void writeDeviceItems();

private:
    QVector<RegisterDeviceItem> deviceItems;
    QFile configFile;
    QString thisKey;
};

#endif // REGISTER_DEVICE_LIST_HPP
