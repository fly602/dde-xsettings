#ifndef XCBUTILS_H
#define XCBUTILS_H

#include <QObject>
#include <xcb/xproto.h>
#include <QVector>
#include <QDebug>

class XcbUtils : public QObject
{
    Q_OBJECT
public:
    static XcbUtils& getInstance();
    xcb_window_t createWindows();
    xcb_atom_t getAtom(const char *name, bool exist=false);
    bool changeWindowPid(xcb_window_t window);
    bool isSelectionOwned(QString prop);
    bool changeSettingProp(QByteArray data);
    void updateXResources(QVector<QPair<QString,QString>> xresourceInfos);
    char* getXResources();
    int  setXResources(char *data, unsigned long length);
    QVector<QPair<QString, QString>> unmarshalXResources(const QString &datas);
    QString marshalXResources(const QVector<QPair<QString, QString>> &infos);
    QByteArray getXcbAtomProperty(xcb_atom_t atom);
    QByteArray xcbPropertyReplyDataToArray(xcb_get_property_reply_t* reply);
    QString getXcbAtomName(xcb_atom_t atom);

private:
    XcbUtils(QObject *parent = nullptr);
    XcbUtils(const XcbUtils&)=delete;
    XcbUtils& operator=(const XcbUtils&)=delete;
private:
    xcb_connection_t    *connection;
    xcb_window_t        window;
};

#endif // XCBUTILS_H
