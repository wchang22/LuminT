#ifndef REGISTERDEVICEMODEL_H
#define REGISTERDEVICEMODEL_H

#include <QAbstractListModel>

class RegisterDeviceModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit RegisterDeviceModel(QObject *parent = nullptr);

    enum
    {
        DeviceIDRole,
        ReadOnlyStatusRole,
        ButtonTextRole,
    };

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    virtual QHash<int, QByteArray> roleNames() const override;

private:
};

#endif // REGISTERDEVICEMODEL_H
