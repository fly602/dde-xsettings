#include "xsettingsadaptor.h"

#include <openssl/evp.h>
#include <QCoreApplication>

#define XSETTINGS_DBUS_SERVICE "org.deepin.dde.test.XSettings1"
#define XSETTINGS_DBUS_PATH "/org/deepin/dde/XSettings1"
#define XSETTINGS_DBUS_IFC "org.deepin.dde.XSettings1"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    DTK_CORE_NAMESPACE::DConfig::setAppId("dde-xsettings");
    XSettings1 * xSettings = new XSettings1();
    new XSettings1Adaptor(xSettings);

    QDBusConnection::sessionBus().registerService(XSETTINGS_DBUS_SERVICE);
    QDBusConnection::sessionBus().registerObject(XSETTINGS_DBUS_PATH, XSETTINGS_DBUS_IFC, xSettings);

    return a.exec();
}
