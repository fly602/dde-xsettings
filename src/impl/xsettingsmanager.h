#ifndef XSETTINGSMANAGER_H
#define XSETTINGSMANAGER_H

#include "../dbus/types/scaleFactors.h"
#include "../dbus/types/arrayOfColor.h"
#include "modules/api/xcbutils.h"
#include "modules/common/common.h"
#include "dconfinfos.h"
#include "modules/api/keyfile.h"

#include <QObject>
#include <QDBusInterface>
#include <QSharedPointer>
#include <DConfig>
#include <QVector>

class XSettingsManager : public QObject
{
    Q_OBJECT
public:
    XSettingsManager();
    ArrayOfColor getColor(const QString &prop);
    int getInteger(const QString& prop);
    double getScaleFactor();
    ScaleFactors getScreenScaleFactors();
    QString getString(const QString& prop);
    QString listProps();
    void setColor(const QString &prop, const ArrayOfColor &v);
    void setInteger(const QString &prop, const int &v);
    void setScreenScaleFactors(const ScaleFactors &factors,bool emitSignal);
    void setString(const QString &prop, const QString &v);
private:
    void updateDPI();
    void updateXResources();
    void updateFirefoxDPI();
    XsValue getSettingValue(QString prop);
    void setSettings(QVector<XsSetting> settings);
    QVector<XsSetting> getSettingsInSchema();
    ScaleFactors parseScreenFactors(QString screenFactors);
    void setGSettingsByXProp(const QString& prop, XsValue value);
    void setSingleScaleFactor(double scale, bool emitSignal);
    void setScaleFactorForPlymouth(int factor, bool emitSignal);
    QString getPlymouthTheme(QString file);
    int getPlymouthThemeScaleFactor(QString theme);
    QString joinScreenScaleFactors(const ScaleFactors &factors);
    void setScreenScaleFactorsForQt(const ScaleFactors &factors);
    void updateGreeterQtTheme(KeyFile& keyFile);
    void cleanUpDdeEnv();
    QMap<QString,QString> loadDDEUserEnv();
    void saveDDEUserEnv(const QMap<QString,QString>& userEnvs);
    QByteArray getSettingPropValue();
private:
    DTK_CORE_NAMESPACE::DConfig     settingDconfig;
    QSharedPointer<QDBusInterface>  greeterInterface;
    QSharedPointer<QDBusInterface>  sysDaemonInterface;
    QVector<int>                    plymouthScalingTasks;
    bool                            plymouthScaling;
    bool                            restartOSD;
    XcbUtils&                       xcbUtils;
    DconfInfos                      dconfInfos;
};

#endif // XSETTINGSMANAGER_H
