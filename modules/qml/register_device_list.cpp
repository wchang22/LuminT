#include <QUuid>

#include "register_device_list.hpp"

RegisterDeviceList::RegisterDeviceList()
    : configFile(CONFIG_FILE_NAME)
    , thisID("")
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
    if (item.deviceID == oldItem.deviceID &&
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
                      .substr(1, 1+DEVICE_ID_SIZE))
                      .c_str(), DEVICE_ID_SIZE);

    configFile.close();
}

void RegisterDeviceList::readDeviceItems()
{
    configFile.open(QIODevice::ReadOnly);

    // first ID is our ID
    thisID = QString(configFile.readLine(DEVICE_ID_SIZE+1));

    int seq = 1;

    while (!configFile.atEnd())
    {
        // Read all IDs and append to deviceItems vector
        QString line(configFile.readLine(DEVICE_ID_SIZE+1));
        deviceItems.append({ line, true, seq++, QStringLiteral("\u2013") });
    }

    configFile.close();
}

void RegisterDeviceList::writeDeviceItems()
{
    configFile.open(QIODevice::WriteOnly);

    // write our ID first
    configFile.write(thisID.toUtf8());

    for (int i = 1; i < deviceItems.length(); i++)
        configFile.write(deviceItems.at(i).deviceID.toUtf8());

    configFile.close();
}

void RegisterDeviceList::insertItem()
{
    if (deviceItems.at(0).deviceID.isEmpty())
        return;

    emit preItemInserted(1);

    static int seq = 0;

    // Generate a new item based on text entered and insert it below
    // the add-new-id-field
    RegisterDeviceItem item;
    item.deviceID = deviceItems.at(0).deviceID;
    item.readOnlyStatus = true;
    item.seq = ++seq;
    item.buttonText = QStringLiteral("\u2013");
    deviceItems.insert(1, item);

    // Clear the add-new-id field
    item.deviceID = QStringLiteral("");
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

bool RegisterDeviceList::checkNewDeviceID()
{
    // New Device IDs must have be 8 characters and
    // cannot be the same as our device ID
    return (deviceItems.first().deviceID.length() == 8 ||
            deviceItems.first().deviceID.length() == 0) &&
            deviceItems.first().deviceID != getThisID();
}

QString RegisterDeviceList::getThisID() const
{
    return thisID;
}

