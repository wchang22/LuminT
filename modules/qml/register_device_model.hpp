#ifndef REGISTERDEVICEMODEL_H
#define REGISTERDEVICEMODEL_H

#include <QAbstractListModel>

class RegisterDeviceList;

class RegisterDeviceModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(RegisterDeviceList *deviceList READ getDeviceList WRITE setDeviceList)

public:
    explicit RegisterDeviceModel(QObject *parent = nullptr);
    ~RegisterDeviceModel();

    enum
    {
        DeviceIDRole,
        ReadOnlyStatusRole,
        SeqRole,
        ButtonTextRole,
    };

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    virtual QHash<int, QByteArray> roleNames() const override;

    RegisterDeviceList *getDeviceList() const;
    void setDeviceList(RegisterDeviceList *value);

private:
    RegisterDeviceList *deviceList;
};

#endif // REGISTERDEVICEMODEL_H
