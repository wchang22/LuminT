#include <QUuid>

#include "register_device_list.hpp"
#include "modules/utilities/utilities.hpp"

RegisterDeviceList::RegisterDeviceList()
    : configFile(LuminT::CONFIG_FILE_NAME)
    , thisKey("")
{
    // Add field for adding new devices
    deviceItems.append({
        QStringLiteral(""),
        false,
        0,
        QStringLiteral("+"),
    });
}

RegisterDeviceList::~RegisterDeviceList()
{
    writeDeviceItems();
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
    if (item.deviceKey == oldItem.deviceKey &&
        item.readOnlyStatus == oldItem.readOnlyStatus &&
        item.seq == oldItem.seq &&
        item.buttonText == oldItem.buttonText)
        return false;

    deviceItems[index] = item;
    return true;
}

void RegisterDeviceList::generateConf()
{
    if (configFile.exists())
        return;

    configFile.open(QIODevice::WriteOnly);

    configFile.write((QUuid::createUuid()
                      .toString().toStdString()
                      .substr(1, 1+LuminT::DEVICE_KEY_SIZE))
                      .c_str(), LuminT::DEVICE_KEY_SIZE);

    configFile.close();
}

void RegisterDeviceList::readDeviceItems()
{
    configFile.open(QIODevice::ReadOnly);

    // first key is our key
    thisKey = QString(configFile.readLine(LuminT::DEVICE_KEY_SIZE+1));

    int seq = 1;

    while (!configFile.atEnd())
    {
        // Read all keyss and append to deviceItems vector
        QString line(configFile.readLine(LuminT::DEVICE_KEY_SIZE+1));
        deviceItems.append({ line, true, seq++, QStringLiteral("\u2013") });
    }

    configFile.close();
}

void RegisterDeviceList::writeDeviceItems()
{
    configFile.open(QIODevice::WriteOnly);

    // write our key first
    configFile.write(thisKey.toUtf8());

    for (int i = 1; i < deviceItems.length(); i++)
        configFile.write(deviceItems.at(i).deviceKey.toUtf8());

    configFile.close();
}

void RegisterDeviceList::insertItem()
{
    if (deviceItems.at(0).deviceKey.isEmpty())
        return;

    emit preItemInserted(1);

    static int seq = 0;

    // Generate a new item based on text entered and insert it below
    // the add-new-key-field
    RegisterDeviceItem item;
    item.deviceKey = deviceItems.at(0).deviceKey;
    item.readOnlyStatus = true;
    item.seq = ++seq;
    item.buttonText = QStringLiteral("\u2013");
    deviceItems.insert(1, item);

    // Clear the add-new-key field
    item.deviceKey = QStringLiteral("");
    item.readOnlyStatus = false;
    item.seq = 0;
    item.buttonText = QStringLiteral("+");
    deviceItems.replace(0, item);

    emit postItemInserted();
}

void RegisterDeviceList::removeItem(int index)
{
    // Find the item that has the sequence number corresponding
    // with the sequence number of the button clicked
    int i;
    for (i = 0; i < deviceItems.length(); i++)
        if (deviceItems[i].seq == index)
            break;

    emit preItemRemoved(i);

    deviceItems.erase(deviceItems.begin()+i);

    emit postItemRemoved();
}

bool RegisterDeviceList::checkNewDeviceKey()
{
    // New Device keys must have be 8 characters and
    // cannot be the same as our device key
    return (deviceItems.first().deviceKey.length() == LuminT::DEVICE_KEY_SIZE ||
            deviceItems.first().deviceKey.length() == 0) &&
            deviceItems.first().deviceKey != getThisKey();
}

QString RegisterDeviceList::getThisKey() const
{
    return thisKey;
}

#include "moc_register_device_list.cpp"
