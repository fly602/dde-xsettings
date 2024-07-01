#ifndef XSETTINGS_H
#define XSETTINGS_H

#include "../dbus/types/scaleFactors.h"
#include "../dbus/types/arrayOfColor.h"
#include "xsettingsmanager.h"

#include <QObject>
#include <QString>

class XSettings1 : public QObject{
    Q_OBJECT

public:
    XSettings1(QObject *parent = nullptr);
    ~XSettings1();

public Q_SLOTS: // METHODS
    ArrayOfColor GetColor(const QString &prop);
    qint32 GetInteger(const QString &prop);
    double GetScaleFactor();
    ScaleFactors GetScreenScaleFactors();
    QString GetString(const QString &prop);
    QString ListProps();
    void SetColor(const QString &prop, const ArrayOfColor &v);
    void SetInteger(const QString &prop, const qint32 &v);
    void SetScaleFactor(const double &scale); 
    void SetScreenScaleFactors(const ScaleFactors &factors);
    void SetString(const QString &prop, const QString &v);

Q_SIGNALS: // SIGNALS
    void SetScaleFactorDone();
    void SetScaleFactorStarted(const QString &type);

private:
    QSharedPointer<XSettingsManager> xSettingsmanger;
};

#endif
