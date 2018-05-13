#include "register_device_model.hpp"
#include "register_device_list.hpp"

#include <QDebug>

RegisterDeviceModel::RegisterDeviceModel(QObject *parent)
    : QAbstractListModel(parent)
    , deviceList(nullptr)
{

}

RegisterDeviceModel::~RegisterDeviceModel()
{
    deviceList->writeDeviceItems();
}

int RegisterDeviceModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid() || !deviceList)
        return 0;

    return deviceList->items().size();
}

QVariant RegisterDeviceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !deviceList)
        return QVariant();

    const RegisterDeviceItem item = deviceList->items().at(index.row());

    switch (role) {
        case DeviceIDRole:
            return QVariant(item.deviceID);
        case ReadOnlyStatusRole:
            return QVariant(item.readOnlyStatus);
        case SeqRole:
            return QVariant(item.seq);
        case ButtonTextRole:
            return QVariant(item.buttonText);
    }
    return QVariant();
}

bool RegisterDeviceModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!deviceList)
        return false;

    RegisterDeviceItem item = deviceList->items().at(index.row());

    switch (role) {
        case DeviceIDRole:
            item.deviceID = value.toString();
            break;
        case ReadOnlyStatusRole:
            item.readOnlyStatus = value.toBool();
            break;
        case SeqRole:
            item.seq = value.toInt();
            break;
        case ButtonTextRole:
            item.buttonText = value.toString();
    }

    if (deviceList->setItemAt(index.row(), item)) {
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags RegisterDeviceModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable;
}

QHash<int, QByteArray> RegisterDeviceModel::roleNames() const
{
    QHash<int, QByteArray> names;

    names[DeviceIDRole] = "deviceID";
    names[ReadOnlyStatusRole] = "readOnlyStatus";
    names[SeqRole] = "seq";
    names[ButtonTextRole] = "buttonText";

    return names;
}

RegisterDeviceList *RegisterDeviceModel::getDeviceList() const
{
    return deviceList;
}

void RegisterDeviceModel::setDeviceList(RegisterDeviceList *value)
{
    beginResetModel();

    if (deviceList)
        deviceList->disconnect(this);

    deviceList = value;

    if (deviceList)
    {
        connect(deviceList, &RegisterDeviceList::preItemInserted, this, [=]() {
            beginInsertRows(QModelIndex(), 1, 1);
        });

        connect(deviceList, &RegisterDeviceList::postItemInserted, this, [=]() {
            endInsertRows();
        });

        connect(deviceList, &RegisterDeviceList::preItemRemoved, this, [=](int index) {
            beginRemoveRows(QModelIndex(), index, index);
        });

        connect(deviceList, &RegisterDeviceList::postItemRemoved, this, [=]() {
            endRemoveRows();
        });
    }

    endResetModel();
}
