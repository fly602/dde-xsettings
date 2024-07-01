#include "xsdatainfo.h"
#include "modules/api/utils.h"
\
#include <QDebug>

XSItemInfo::XSItemInfo(QByteArray& datas)
{
    unMarshalXSItemInfoData(datas);
}

XSItemInfo::XSItemInfo(const QString& prop, const XsValue& xsValue)
{
    if(std::get_if<int>(&xsValue) != nullptr)
    {
        initXSItemInfoInteger(prop,std::get<int>(xsValue));
    }else if (std::get_if<QString>(&xsValue) != nullptr) {
        initXSItemInfoString(prop,std::get<QString>(xsValue));
    }else if (std::get_if<ColorValueInfo>(&xsValue) != nullptr) {
        initXSItemInfoColor(prop,std::get<ColorValueInfo>(xsValue));
    }else {
        qDebug()<<"";
    }
}

void XSItemInfo::unMarshalXSItemInfoData(QByteArray& datas)
{

    readXSItemHeader(datas);

    int tempInt = 0;
    QString tempString = 0;
    ColorValueInfo tempColor;

    switch (head.type) {
    case HeadTypeInteger:
        Utils::readInteger(datas,tempInt);
        value = tempInt;
        break;
    case HeadTypeString:
        int length;
        Utils::readInteger(datas,length);
        Utils::readString(datas,tempString, length);
        Utils::readSkip(datas,Utils::getPad(length));
        value = tempString;
        break;
    case HeadTypeColor:
        Utils::readInteger(datas,tempColor.red);
        Utils::readInteger(datas,tempColor.green);
        Utils::readInteger(datas,tempColor.blue);
        Utils::readInteger(datas,tempColor.alpha);
        value = tempColor;
        break;
    default:
        //todo

        ;
    }
}

void XSItemInfo::marshalXSItemInfoData(QByteArray& datas)
{
    writeXSItemHeader(datas);

    switch (head.type) {
    case HeadTypeInteger:
        if(std::get_if<int>(&value) == nullptr)
        {
            break;
        }
        Utils::writeInteger(datas,std::get<int>(value));
        break;
    case HeadTypeString:
        int length;
        Utils::writeInteger(datas,length);
        if(std::get_if<QString>(&value) == nullptr)
        {
            break;
        }
        Utils::writeString(datas,std::get<QString>(value));
        Utils::writeSkip(datas,Utils::getPad(length));
        break;
    case HeadTypeColor:
        ColorValueInfo  *colorValue;
        colorValue = std::get_if<ColorValueInfo>(&value);
        if(colorValue == nullptr)
        {
            break;
        }
        Utils::writeInteger(datas,colorValue->red);
        Utils::writeInteger(datas,colorValue->green);
        Utils::writeInteger(datas,colorValue->blue);
        Utils::writeInteger(datas,colorValue->alpha);
        break;
    default:
        //todo

        ;
    }
}

void XSItemInfo::readXSItemHeader(QByteArray& datas)
{
    Utils::readInteger(datas,head.type);
    Utils::readSkip(datas,1);
    Utils::readInteger(datas,head.length);
    Utils::readString(datas,head.name,static_cast<int>(head.length));
    Utils::readSkip(datas,Utils::getPad(static_cast<int>(head.length)));
    Utils::readInteger(datas,head.lastChangeSerial);
}

void XSItemInfo::writeXSItemHeader(QByteArray& datas)
{
    Utils::writeInteger(datas,head.type);
    Utils::writeSkip(datas,1);
    Utils::writeInteger(datas,head.length);
    Utils::writeString(datas,head.name);
    Utils::writeSkip(datas,Utils::getPad(static_cast<int>(head.length)));
    Utils::writeInteger(datas,head.lastChangeSerial);
}

QString XSItemInfo::getHeadName()
{
    return head.name;
}

XsValue XSItemInfo::getValue()
{
    return value;
}

void XSItemInfo::modifyProperty(const XsSetting& setting)
{
    head.lastChangeSerial++;
    value = setting.value;
}

void XSItemInfo::initXSItemInfoInteger(const QString& prop, int value)
{
    head.name = prop;
    head.length = prop.length();
    head.lastChangeSerial =1;
    head.type = HeadTypeInteger;

    this->value = value;
}

void XSItemInfo::initXSItemInfoString(const QString& prop,QString value)
{
    head.name = prop;
    head.length = prop.length();
    head.lastChangeSerial =1;
    head.type = HeadTypeString;

    this->value = value;
}

void XSItemInfo::initXSItemInfoColor(const QString& prop,ColorValueInfo value)
{
    head.name = prop;
    head.length = prop.length();
    head.lastChangeSerial =1;
    head.type = HeadTypeColor;

    this->value = value;
}


XSDataInfo::XSDataInfo(QByteArray& datas)
{
    unMarshalSettingData(datas);
}

void XSDataInfo::unMarshalSettingData(QByteArray& datas)
{
    if(datas.isEmpty())
    {
        byteOrder   = 0;
        serial      = 0;
        numSettings = 0;
        return;
    }

    Utils::readInteger(datas,byteOrder);
    Utils::readSkip(datas,3);
    Utils::readInteger(datas,serial);
    Utils::readInteger(datas,numSettings);

    for(uint32_t i=0;i<numSettings;i++)
    {
        QSharedPointer<XSItemInfo> info(new XSItemInfo(datas));
        items.push_back(info);
    }
}

QByteArray XSDataInfo::marshalSettingData()
{
    QByteArray array;

    Utils::writeInteger(array,byteOrder);
    Utils::writeSkip(array,3);
    Utils::writeInteger(array,serial);
    Utils::writeInteger(array,numSettings);

    for(auto item:items)
    {
        item->marshalXSItemInfoData(array);
    }

    return array;
}

QString XSDataInfo::listProps()
{
    QString content("[");

    for(int i = 0;i<items.length();i++)
    {
        if(i != 0)
        {
            content +=",";
        }
        content += items[i]->getHeadName();    
    }
    content.push_back("]");
    return content;
}

QSharedPointer<XSItemInfo> XSDataInfo::getPropItem(QString prop)
{
    for(auto item:items)
    {
        if(item->getHeadName() == prop)
        {
            return item;
        }
    }

    return QSharedPointer<XSItemInfo>();
}

void XSDataInfo::inserItem(QSharedPointer<XSItemInfo> itemInfo)
{
    items.push_back(itemInfo);
}

void XSDataInfo::increaseSerial()
{
    serial++;
}

void XSDataInfo::increaseNumSettings()
{
    numSettings++;
}
