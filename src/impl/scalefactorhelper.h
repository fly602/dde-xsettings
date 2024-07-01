#ifndef SCALEFACTORHELPER_H
#define SCALEFACTORHELPER_H

#include <QObject>
#include <QSharedPointer>
#include <QDBusInterface>
#include <functional>
#include <QMap>

class ScaleFactorHelper:public QObject
{
    typedef std::function<void (QMap<QString,float>)> CHANGECALLBACK;
public:
    ScaleFactorHelper();

private:
    QSharedPointer<QDBusInterface> sysDisplay;
    CHANGECALLBACK                 callback;
};

#endif // SCALEFACTORHELPER_H
