#include "xsettings1.h"

XSettings1::XSettings1(QObject *parent)
    :xSettingsmanger(new XSettingsManager())
{
    registerScaleFactorsMetaType();
    registerArrayOfColorMetaType();
}

XSettings1::~XSettings1()
{

}

QList<quint16> XSettings1::GetColor(const QString &prop)
{
    return xSettingsmanger->getColor(prop);
}

qint32 XSettings1::GetInteger(const QString &prop)
{
    return xSettingsmanger->getInteger(prop);
}

double XSettings1::GetScaleFactor()
{
    return xSettingsmanger->getScaleFactor();
}

ScaleFactors XSettings1::GetScreenScaleFactors()
{
    return xSettingsmanger->getScreenScaleFactors();
}

QString XSettings1::GetString(const QString &prop)
{
    return xSettingsmanger->getString(prop);
}

QString XSettings1::ListProps()
{
    return xSettingsmanger->listProps();
}

void XSettings1::SetColor(const QString &prop, const QList<quint16> &v)
{
    xSettingsmanger->setColor(prop,v);
}

void XSettings1::SetInteger(const QString &prop, const qint32 &v)
{
    xSettingsmanger->setInteger(prop,v);
}

void XSettings1::SetScaleFactor(const double &scale)
{
    ScaleFactors factors;
    factors["ALL"] = scale;
    xSettingsmanger->setScreenScaleFactors(factors,true);
}

void XSettings1::SetScreenScaleFactors(const ScaleFactors &factors)
{
    xSettingsmanger->setScreenScaleFactors(factors,true);
}

void XSettings1::SetString(const QString &prop, const QString &v)
{
    xSettingsmanger->setString(prop,v);
}
