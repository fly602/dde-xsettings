#ifndef DCONFINFOS_H
#define DCONFINFOS_H

#include "xsdatainfo.h"

#include <QObject>
#include <functional>
#include <DConfig>

class DconfInfo : public QObject
{
    Q_OBJECT
    typedef     std::function<XsValue (XsValue& )> ConverFun;

public:
    DconfInfo(QString dconfKey,QString xsKey,DconfValueType dconfType, int xsType=0);
    void setGsToXsFunc(ConverFun func);
    void setXsToGsFunc(ConverFun func);
    XsValue getValue(const DTK_CORE_NAMESPACE::DConfig& dconf);
    bool setValue(DTK_CORE_NAMESPACE::DConfig& dconf,XsValue& value);
    ConverFun getGsToXsFunc(ConverFun func);
    ConverFun getXsToGsFunc(ConverFun func);
    XsValue   convertStrToDouble(XsValue& value);
    XsValue   convertDoubleToStr(XsValue& value);
    XsValue   convertStrToColor(XsValue& value);
    XsValue   convertColorToStr(XsValue& value);
    QString   getDconfKey();
    QString   getXsetKey();
    uint8_t   getKeySType();
private:
    QString             dconfKey;
    DconfValueType      dconfType;
    QString             xsKey;
    uint8_t             xsType;
    ConverFun           convertGsToXs;
    ConverFun           convertXsToGs;
};


class DconfInfos : public QObject{
    Q_OBJECT

public:
    DconfInfos();
    QSharedPointer<DconfInfo> getByDconfKey(const QString& dconfKey);
    QSharedPointer<DconfInfo> getByXSKey(const QString& xsettingKey);

private:
    QVector<QSharedPointer<DconfInfo>> dconfArray;
};

#endif // DCONFINFOS_H
