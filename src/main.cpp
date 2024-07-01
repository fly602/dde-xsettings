#include "xsettingsadaptor.h"

#include <openssl/evp.h>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    XSettings1 * xSettings = new XSettings1();
    new XSettings1Adaptor(xSettings);

    QDBusConnection::sessionBus().registerService("com.deepin.XSettings1");
    QDBusConnection::sessionBus().registerObject("/com/deepin/XSettings1", "org.deepin.XSettings1", xSettings);

    return a.exec();
}
