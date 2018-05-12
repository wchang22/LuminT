#include "register_device_model.hpp"

RegisterDeviceModel::RegisterDeviceModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

QVariant RegisterDeviceModel::headerData(int section, Qt::Orientation orientation, int role) const
{

}

bool RegisterDeviceModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (value != headerData(section, orientation, role)) {
        // FIXME: Implement me!
        emit headerDataChanged(orientation, section, section);
        return true;
    }
    return false;
}

int RegisterDeviceModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    // FIXME: Implement me!
    return 10;
}

QVariant RegisterDeviceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    // FIXME: Implement me!
    switch (role) {
        case DeviceIDRole:
            return QVariant(QStringLiteral("12345678"));
        case ReadOnlyStatusRole:
            return QVariant(true);
        case ButtonTextRole:
            return QVariant("-");
    }
    return QVariant();
}

bool RegisterDeviceModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value) {
        // FIXME: Implement me!
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags RegisterDeviceModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable; // FIXME: Implement me!
}

bool RegisterDeviceModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endInsertRows();
}

bool RegisterDeviceModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endRemoveRows();
}

QHash<int, QByteArray> RegisterDeviceModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names[DeviceIDRole] = "deviceID";
    names[ReadOnlyStatusRole] = "readOnlyStatus";
    names[ButtonTextRole] = "buttonText";
    return names;
}
