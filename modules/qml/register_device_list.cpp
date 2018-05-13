#include <QDebug>

#include "register_device_list.hpp"

RegisterDeviceList::RegisterDeviceList()
    : configFile(CONFIG_FILE_NAME)
{
    deviceItems.append({
        QStringLiteral(""),
        false,
        0,
        QStringLiteral("+"),
    });
}

RegisterDeviceList::~RegisterDeviceList()
{

}

QVector<RegisterDeviceItem> RegisterDeviceList::items() const
{
    return deviceItems;
}

bool RegisterDeviceList::setItemAt(int index, const RegisterDeviceItem &item)
{
    if (index < 0 || index >= deviceItems.size())
        return false;

    const RegisterDeviceItem &oldItem = deviceItems.at(index);
    if (item.deviceID == oldItem.deviceID &&
        item.readOnlyStatus == oldItem.readOnlyStatus &&
        item.seq == oldItem.seq &&
        item.buttonText == oldItem.buttonText)
        return false;

    deviceItems[index] = item;
    return true;
}

void RegisterDeviceList::readDeviceItems()
{
    configFile.open(QIODevice::ReadOnly);
    configFile.skip(DEVICE_ID_SIZE);
    deviceItems.erase(deviceItems.begin()+1, deviceItems.end());

    int seq = 1;

    while (!configFile.atEnd())
    {
        QString line(configFile.readLine(DEVICE_ID_SIZE+1));
        deviceItems.append({ line, true, seq++, QStringLiteral("\u2013") });
    }

    configFile.close();
}

void RegisterDeviceList::writeDeviceItems()
{
    configFile.open(QIODevice::ReadOnly);

    QByteArray ownID = configFile.readLine(DEVICE_ID_SIZE+1);

    configFile.close();
    configFile.open(QIODevice::WriteOnly);
    configFile.write(ownID);

    for (int i = 1; i < deviceItems.length(); i++)
        configFile.write(deviceItems.at(i).deviceID.toUtf8());

    configFile.close();
}

void RegisterDeviceList::insertItem()
{
    if (deviceItems.at(0).deviceID.isEmpty())
        return;

    emit preItemInserted();

    static int seq = 0;

    RegisterDeviceItem item;
    item.deviceID = deviceItems.at(0).deviceID;
    item.readOnlyStatus = true;
    item.seq = ++seq;
    item.buttonText = QStringLiteral("\u2013");
    deviceItems.insert(1, item);

    item.deviceID = QStringLiteral("");
    item.readOnlyStatus = false;
    item.seq = 0;
    item.buttonText = QStringLiteral("+");
    deviceItems.replace(0, item);

    emit postItemInserted();
}

void RegisterDeviceList::removeItem(int index)
{
    int i;
    for (i = 0; i < deviceItems.length(); i++)
        if (deviceItems[i].seq == index)
            break;

    emit preItemRemoved(i);

    deviceItems.erase(deviceItems.begin()+i);

    emit postItemRemoved();
}

bool RegisterDeviceList::checkNewDeviceID()
{
    return deviceItems.first().deviceID.length() == 8 ||
           deviceItems.first().deviceID.length() == 0;
}

