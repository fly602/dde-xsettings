#include "xsettingsmanager.h"
#include "xsdatainfo.h"
#include "modules/api/utils.h"

#include <QtMath>
#include <QGSettings>
#include <QDebug>
#include <QDir>
#include <fcntl.h>

const static int DPI_FALLBACK = 96;
const static int BASE_CURSORSIZE = 24;
const static QString PLYMOUTH_CONFIGFILE = "/etc/plymouth/plymouthd.conf";

XSettingsManager::XSettingsManager()
    :settingDconfig("com.deepin.xsettings")
    ,greeterInterface(new QDBusInterface("com.deepin.daemon.Greeter",
                                      "/com/deepin/daemon/Greeter",
                                      "com.deepin.daemon.Greeter",
                                      QDBusConnection::systemBus()))
    ,sysDaemonInterface(new QDBusInterface("com.deepin.daemon.Daemon",
                                      "/com/deepin/daemon/Daemon",
                                      "com.deepin.daemon.Daemon",
                                      QDBusConnection::systemBus()))
    ,xcbUtils(XcbUtils::getInstance())
{
    setSettings(getSettingsInSchema());
}

ArrayOfColor XSettingsManager::getColor(const QString &prop)
{
    ArrayOfColor arrayOfColor;
    XsValue value = getSettingValue(prop);

    if(std::get_if<ColorValueInfo>(&value)==nullptr)
    {
        return arrayOfColor;
    }
    ColorValueInfo color = std::get<ColorValueInfo>(value);
    arrayOfColor.push_back(color.red);
    arrayOfColor.push_back(color.green);
    arrayOfColor.push_back(color.blue);
    arrayOfColor.push_back(color.alpha);

    return arrayOfColor;
}

int XSettingsManager::getInteger(const QString& prop)
{
     XsValue value =getSettingValue(prop);

     if(std::get_if<int>(&value) == nullptr)
     {
         return 0;
     }

     return std::get<int>(value);
}

double XSettingsManager::getScaleFactor()
{
    if(settingDconfig.isValid())
    {
        return  settingDconfig.value(gsKeyScaleFactor).toDouble();
    }

    return 0;
}

ScaleFactors XSettingsManager::getScreenScaleFactors()
{
    if(settingDconfig.isValid())
    {
       QString factors = settingDconfig.value(gsKeyIndividualScaling).toString();
       return parseScreenFactors(factors);
    }

    return ScaleFactors();
}

QString XSettingsManager::getString(const QString& prop)
{
    XsValue value =getSettingValue(prop);

    if(std::get_if<QString>(&value) == nullptr)
    {
        return 0;
    }

    return std::get<QString>(value);
}

QString XSettingsManager::listProps()
{
    QByteArray datas = xcbUtils.getSettingPropValue();

    XSDataInfo xSDataInfo(datas);

    return xSDataInfo.listProps();
}

void XSettingsManager::setColor(const QString &prop, const ArrayOfColor &v)
{
    if(v.size() != 4)
    {
        return;
    }
    ColorValueInfo color;
    color.red   = v[0];
    color.green = v[1];
    color.blue  = v[2];
    color.alpha = v[3];

    XsSetting xsValue;
    xsValue.prop = prop;
    xsValue.type = HeadTypeColor;
    xsValue.value = color;


    QVector<XsSetting> xsSetting{xsValue};
    setSettings(xsSetting);

    setGSettingsByXProp(prop,color);
}

void XSettingsManager::setInteger(const QString &prop, const qint32 &v)
{
    XsSetting xsValue;
    xsValue.prop = prop;
    xsValue.type = HeadTypeInteger;
    xsValue.value = v;


    QVector<XsSetting> xsSetting{xsValue};
    setSettings(xsSetting);

    setGSettingsByXProp(prop,v);
}

void XSettingsManager::setScreenScaleFactors(const ScaleFactors &factors, bool emitSignal)
{
    if(factors.isEmpty())
    {
        return;
    }

    for(auto iter:factors.keys())
    {
        if(factors[iter]<0)
        {
            return;
        }
    }

    // err := m.dsfHelper.SetScaleFactors(factors)

    double singleFactor=0;
    if(factors.size()==1)
    {
        singleFactor = factors.first();
    }else {
        if(factors.count("All") ==1)
        {
            singleFactor = factors["All"];
        }else {
            singleFactor = 1;
        }
    }
    setSingleScaleFactor(singleFactor,emitSignal);

    QString factorsJoined = joinScreenScaleFactors(factors);
    if(settingDconfig.isValid())
    {
        settingDconfig.setValue(gsKeyIndividualScaling,factorsJoined);
    }

    setScreenScaleFactorsForQt(factors);

}

void XSettingsManager::setString(const QString &prop, const QString &v)
{
    XsSetting xsValue;
    xsValue.prop = prop;
    xsValue.type = HeadTypeString;
    xsValue.value = v;


    QVector<XsSetting> xsSetting{xsValue};
    setSettings(xsSetting);

    setGSettingsByXProp(prop,v);
}

void XSettingsManager::updateDPI()
{
    double scale = 1;
    QVector<XsSetting> xsSettngVec;
    int scaledDpi = static_cast<int>((DPI_FALLBACK * 1024)*scale);
    int cursorSize;
    int windowScale;

    if(settingDconfig.isValid())
    {   bool bOk = false;
        double tempScaleFactor = settingDconfig.value(gsKeyScaleFactor).toDouble(&bOk);
        if(bOk)
        {
            if(tempScaleFactor <= 0)
            {
                scale = 1;
            }else {
                scale = tempScaleFactor;
            }
        }

        bOk = false;
        int tempXftDpi = settingDconfig.value(gsKeyXftDpi).toInt(&bOk);
        if(bOk)
        {
            if(tempXftDpi != scaledDpi)
            {
                settingDconfig.setValue(gsKeyXftDpi,scale);
                XsSetting setting;
                setting.prop = gsKeyXftDpi;
                setting.value = scaledDpi;
                setting.type = HeadTypeInteger;

                xsSettngVec.push_back(setting);
            }
        }

        bOk = false;
        windowScale = settingDconfig.value(gsKeyWindowScale).toInt(&bOk);
        if(bOk)
        {
            if(windowScale > 1)
            {
                scaledDpi = static_cast<int>(DPI_FALLBACK * 1024);
            }
        }

        bOk = false;
        int tempGtkCursorThemeSize = settingDconfig.value(gsKeyGtkCursorThemeSize).toInt(&bOk);
        if(bOk)
        {
            cursorSize = tempGtkCursorThemeSize;
        }
    }

    int  windowScalingFactor =getInteger("Gdk/WindowScalingFactor");
    if(windowScalingFactor != windowScale)
    {
        XsSetting sWindowScalingFactor;
        sWindowScalingFactor.prop = "Gdk/WindowScalingFactor";
        sWindowScalingFactor.value = windowScale;
        sWindowScalingFactor.type = HeadTypeInteger;

        xsSettngVec.push_back(sWindowScalingFactor);

        XsSetting sDpi;
        sDpi.prop = "Gdk/UnscaledDPI";
        sDpi.value = scaledDpi;
        sDpi.type = HeadTypeInteger;

        xsSettngVec.push_back(sDpi);

        XsSetting sCursorThemeSize;
        sCursorThemeSize.prop = cursorSize;
        sCursorThemeSize.value = "Gtk/CursorThemeSize";
        sCursorThemeSize.type = HeadTypeInteger;

        xsSettngVec.push_back(sCursorThemeSize);
    }

    if(!xsSettngVec.isEmpty())
    {
        setSettings(xsSettngVec);
        updateXResources();
    }

}

void XSettingsManager::updateXResources()
{
    QVector<QPair<QString,QString>> xresourceInfos;
    if(settingDconfig.isValid())
    {
        xresourceInfos.push_back(qMakePair(QString("Xcursor.theme"),settingDconfig.value("gtk-cursor-theme-name").toString()));
        xresourceInfos.push_back(qMakePair(QString("Xcursor.size"),QString::number(settingDconfig.value(gsKeyGtkCursorThemeSize).toInt())));

        double scaleFactor = settingDconfig.value(gsKeyScaleFactor).toDouble();
        int xftDpi =static_cast<int>(DPI_FALLBACK * scaleFactor);
        xresourceInfos.push_back(qMakePair(QString("Xft.dpi"), QString::number(xftDpi)));
    }


    xcbUtils.updateXResources(xresourceInfos);
}

XsValue XSettingsManager::getSettingValue(QString prop)
{
    QByteArray datas = xcbUtils.getSettingPropValue();

    XSDataInfo xsInfo(datas);

    QSharedPointer<XSItemInfo> item = xsInfo.getPropItem(prop);
    if(item.isNull())
    {
        return  XsValue();
    }

    return item->getValue();
}

void XSettingsManager::setSettings(QVector<XsSetting> settings)
{
    QByteArray datas = xcbUtils.getSettingPropValue();

    XSDataInfo xsInfo(datas);
    xsInfo.increaseSerial();

    for(auto xsettingItem : settings)
    {
        QSharedPointer<XSItemInfo> xsItem = xsInfo.getPropItem(xsettingItem.prop);
        if(xsItem)
        {
            xsItem->modifyProperty(xsettingItem);
            continue;
        }

        QSharedPointer<XSItemInfo> xSItemInfo(new XSItemInfo(xsettingItem.prop,xsettingItem.value));
        xsInfo.inserItem(xSItemInfo);
        xsInfo.increaseNumSettings();
    }

    QByteArray value = xsInfo.marshalSettingData();

    xcbUtils.changeSettingProp(value);
}

QVector<XsSetting> XSettingsManager::getSettingsInSchema()
{
    QVector<XsSetting> xsSettingVec;
    if(!settingDconfig.isValid())
    {
        return xsSettingVec;
    }

    QStringList keys = settingDconfig.keyList();
    for(auto key:keys)
    {
        QSharedPointer<DconfInfo> dconfInfo = dconfInfos.getByXSKey(key);
        if(dconfInfo.isNull())
        {
            continue;
        }

        XsValue value = dconfInfo->getValue(settingDconfig);
        if(!Utils::hasXsValue(value))
        {
            continue;
        }

        XsSetting xsStting;
        xsStting.type = dconfInfo->getKeySType();
        xsStting.prop = dconfInfo->getXsetKey();
        xsStting.value = value;

        xsSettingVec.push_back(xsStting);
    }
    return xsSettingVec;
}

ScaleFactors XSettingsManager::parseScreenFactors(QString screenFactors)
{
    ScaleFactors factorMap;

    QStringList factors = screenFactors.split(";");
    for(auto factor:factors)
    {
        QStringList kv = factor.split("=");
        if(kv.size()!=2)
        {
            continue;
        }
        bool bOk = false;
        double value = kv[1].toDouble(&bOk);
        if(bOk)
        {
            factorMap[kv[0]] = value;
        }
    }

    return factorMap;
}

void XSettingsManager::setGSettingsByXProp(const QString& prop, XsValue value)
{
    QSharedPointer<DconfInfo> info = dconfInfos.getByXSKey(prop);
    if(info.isNull())
    {
        return;
    }

    info->setValue(settingDconfig,value);
}

void XSettingsManager::setSingleScaleFactor(double scale, bool emitSignal)
{
    int cursorSize;
    int windowScale = qFloor((scale + 0.3) * 10)/10;
    if(windowScale < 1)
    {
        windowScale = 1;
    }

    if(settingDconfig.isValid())
    {
        settingDconfig.setValue(gsKeyScaleFactor,scale);



        int oldWindowScale = settingDconfig.value(gsKeyWindowScale).toInt();
        if(windowScale != oldWindowScale)
        {
            settingDconfig.setValue(gsKeyWindowScale, windowScale);
        }

        cursorSize = static_cast<int>(BASE_CURSORSIZE * scale);
        settingDconfig.setValue(gsKeyGtkCursorThemeSize, cursorSize);
    }

    if(QGSettings::isSchemaInstalled(WRAPSCHEMA))
    {
        QGSettings gsWrapGDI(WRAPSCHEMA);
        gsWrapGDI.set("cursor-size",cursorSize);
    }

    setScaleFactorForPlymouth(windowScale,emitSignal);
}

void XSettingsManager::setScaleFactorForPlymouth(int factor, bool emitSignal)
{
    if(factor > 2)
    {
        factor = 2;
    }

    QString theme = getPlymouthTheme(PLYMOUTH_CONFIGFILE);
    int  currentFactor = getPlymouthThemeScaleFactor(theme);
    if(currentFactor == factor)
    {
        // m.emitSignalSetScaleFactor(true, emitSignal)
        return;
    }

    // m.emitSignalSetScaleFactor(true, emitSignal)
    sysDaemonInterface->call("ScalePlymouth",static_cast<uint32_t>(factor));
     // m.emitSignalSetScaleFactor(true, emitSignal)
}

QString XSettingsManager::getPlymouthTheme(QString file)
{
    KeyFile keyFile;
    bool bSuccess = keyFile.loadFile(file);
    if(!bSuccess)
    {
        return "";
    }

    return keyFile.getStr("Daemon", "Theme");
}

int XSettingsManager::getPlymouthThemeScaleFactor(QString theme)
{
    if(theme == "deepin-logo"|| theme == "deepin-ssd-logo" || theme == "uos-ssd-logo")
    {
        return 1;
    }else if (theme == "deepin-hidpi-logo"|| theme == "deepin-hidpi-ssd-logo" || theme == "uos-hidpi-ssd-logo") {
        return 2;
    }else {
        return 0;
    }
}

QString XSettingsManager::joinScreenScaleFactors(const ScaleFactors &factors)
{
    QString value;

    for(auto key:factors.keys())
    {
        value = QString::asprintf("%s=%.2f;",key.toStdString().c_str(),factors[key]);
    }

    return value;
}

void XSettingsManager::setScreenScaleFactorsForQt(const ScaleFactors &factors)
{
    if(factors.isEmpty())
    {
        return;
    }

    QString fileName = Utils::GetUserConfigDir() + "/deepin/qt-theme.ini";
    KeyFile keyFile;
    if(!keyFile.loadFile(fileName))
    {
        qWarning()<<"failed to load qt-theme.ini:";
    }

    QString value;
    if(factors.size()==1)
    {
        value = QString::number(factors.first());
    }else {
        value = joinScreenScaleFactors(factors);
    }

    keyFile.setKey(qtThemeSection,qtThemeKeyScaleFactor,value);
    keyFile.deleteKey(qtThemeSection,qtThemeKeyScaleFactor);
    keyFile.setKey(qtThemeSection,qtThemeKeyScaleLogicalDpi,"-1,-1");

    QFile qfile(fileName);
    if (!qfile.exists()) {
        QDir dir(fileName.left(fileName.lastIndexOf("/")));
        dir.mkpath(fileName.left(fileName.lastIndexOf("/")));
        qInfo() << "mkpath" << fileName;
    }

    qfile.open(QIODevice::WriteOnly);
    qfile.close();

    keyFile.saveToFile(fileName);

    updateGreeterQtTheme(keyFile);

    cleanUpDdeEnv();
}

void XSettingsManager::updateGreeterQtTheme(KeyFile& keyFile)
{
    QFile file("/tmp/startdde-qt-theme-");
    if(!file.open(QIODevice::WriteOnly))
    {
        return;
    }

    keyFile.setKey(qtThemeSection,qtThemeKeyScaleLogicalDpi,"96,96");
    keyFile.saveToFile(file.fileName());

    int fd = open("/tmp/startdde-qt-theme-",O_WRONLY);
    greeterInterface->call("UpdateGreeterQtTheme",fd);
}

void XSettingsManager::cleanUpDdeEnv()
{
    bool bNeedSave = false;
    QMap<QString,QString> envMap = loadDDEUserEnv();
    QVector keyEnvs{"QT_SCALE_FACTOR",
                    "QT_SCREEN_SCALE_FACTORS",
                    "QT_AUTO_SCREEN_SCALE_FACTOR",
                    "QT_FONT_DPI",
                    "DEEPIN_WINE_SCALE"};

    for(auto env :keyEnvs)
    {
        if(envMap.find(env) != envMap.end())
        {
            bNeedSave = true;
            envMap.erase(envMap.find(env));
        }
    }

    if(bNeedSave)
    {
        saveDDEUserEnv(envMap);
    }
}

QMap<QString,QString> XSettingsManager::loadDDEUserEnv()
{
    QMap<QString,QString> result;

    QString envFile = Utils::getUserHomeDir()+"/.dde_env";
    QFile file(envFile);
    if(!file.open(QIODevice::ReadOnly)){
        return result;
    }

    while (!file.atEnd()) {
        QString line = file.readLine();
        if(line.front() == '#')
        {
            continue;
        }
        QStringList match = line.split(' ');
        if(match.length() == 3)
        {
            result[match[1]]=match[2];
        }
    }

    return result;
}

void XSettingsManager::saveDDEUserEnv(const QMap<QString,QString>& userEnvs)
{
    QString envFile = Utils::getUserHomeDir()+"/.dde_env";
    QFile file(envFile);
    if(!file.open(QFile::WriteOnly|QFile::Truncate)){
        return;
    }

    QString text = QString::asprintf("# DDE user env file, bash script\n");
    for(auto key :userEnvs.keys())
    {
        text +=QString::asprintf("export %s=%s;\n",key.toStdString().c_str(),userEnvs[key].toStdString().c_str());
    }

    file.write(text.toLatin1(),text.length());
    file.close();
}




