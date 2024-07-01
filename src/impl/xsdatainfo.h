#ifndef XSDATAINFO_H
#define XSDATAINFO_H

#include "modules/common/common.h"

#include <QObject>
#include <QVector>
#include <QSharedPointer>

class XSItemInfo : public QObject
{
    Q_OBJECT
public: 
    XSItemInfo(QByteArray& datas);
    XSItemInfo(const QString& prop, const XsValue& xsValue);
    void unMarshalXSItemInfoData(QByteArray& datas);
    void marshalXSItemInfoData(QByteArray& datas);
    void readXSItemHeader(QByteArray& datas);
    void writeXSItemHeader(QByteArray& datas);
    QString getHeadName();
    XsValue getValue();
    void modifyProperty(const XsSetting& setting);
private:
    void initXSItemInfoInteger(const QString& prop,int value);
    void initXSItemInfoString(const QString& prop,QString value);
    void initXSItemInfoColor(const QString& prop,ColorValueInfo value);
private:
    ItemHeader      head;
    XsValue         value;
};

class XSDataInfo : public QObject
{
    Q_OBJECT

public:
    explicit XSDataInfo(QByteArray& datas);
    void unMarshalSettingData(QByteArray& datas);
    QByteArray marshalSettingData();
    QString listProps();
    QSharedPointer<XSItemInfo> getPropItem(QString prop);
    void inserItem(QSharedPointer<XSItemInfo> itemInfo);
    void increaseSerial();
    void increaseNumSettings();
private:
    uint8_t                             byteOrder;
    uint32_t                            serial;
    uint32_t                            numSettings;
    QVector<QSharedPointer<XSItemInfo>> items;
};

#endif // XSDATAINFO_H
