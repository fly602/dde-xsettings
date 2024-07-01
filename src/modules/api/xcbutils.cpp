#include "xcbutils.h"

#include <unistd.h>
#include <xcb/xcb.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/X.h>
#include <stdio.h>
#include <stdlib.h>

XcbUtils& XcbUtils::getInstance()
{
    static XcbUtils xcbUtils;
    return xcbUtils;
}

XcbUtils::XcbUtils(QObject *parent)
    :QObject(parent)
{
    connection = xcb_connect(nullptr, nullptr);
    window = createWindows();
}
xcb_window_t XcbUtils::createWindows()
{
    /* Get the first screen */
    const xcb_setup_t      *setup  = xcb_get_setup (connection);
    xcb_screen_iterator_t   iter   = xcb_setup_roots_iterator (setup);
    xcb_screen_t           *screen = iter.data;

    /* Create the window */
    xcb_window_t window = xcb_generate_id (connection);

    xcb_create_window (connection,                    /* Connection          */
                               XCB_COPY_FROM_PARENT,                    /* depth (same as root)*/
                               window,                        /* window Id           */
                               screen->root,                  /* parent window       */
                               0, 0,                          /* x, y                */
                               1, 1,                      /* width, height       */
                               0,                            /* border_width        */
                               XCB_WINDOW_CLASS_INPUT_OUTPUT, /* class               */
                               screen->root_visual,           /* visual              */
                               0, nullptr);                     /* masks, not used yet */


    /* Map the window on the screen */
    xcb_map_window (connection, window);
    xcb_flush(connection);
    xcb_atom_t atom = getAtom("_XSETTINGS_S0");

    changeWindowPid(window);

    xcb_set_selection_owner_checked(connection,window,atom,XCB_CURRENT_TIME);

    return window;
}

xcb_atom_t XcbUtils::getAtom(const char *name, bool exist)
{
    xcb_intern_atom_cookie_t cookie = xcb_intern_atom(connection,exist,strlen(name),name);

    xcb_generic_error_t *err = nullptr;
    xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(connection,cookie,&err);
    if(err != nullptr)
    {
        //todo
        return 0;
    }

    xcb_atom_t atom = reply->atom;
    free(reply);

    return atom;
}

bool XcbUtils::changeWindowPid(xcb_window_t window)
{
   pid_t pid = getpid();

   xcb_atom_t atom = getAtom("_NET_WM_PID");
   QByteArray pidArray;
   pidArray.push_back(char(pid));
   pidArray.push_back(char(pid >> 8));
   pidArray.push_back(char(pid >> 16));
   pidArray.push_back(char(pid >> 24));
   int32_t data[4];
   data[0] = int32_t(0xff & pid);
   data[1] = int32_t(0xff & (pid >> 8));
   data[2] = int32_t(0xff & (pid >> 16));
   data[3] = int32_t(0xff & (pid >> 24));

   xcb_change_property_checked(connection,XCB_PROP_MODE_REPLACE,window,atom,XCB_ATOM_CARDINAL,32,
                               4, pidArray.toStdString().c_str());

   return true;
}

bool XcbUtils::isSelectionOwned(QString prop)
{
    xcb_atom_t atom = getAtom(prop.toStdString().c_str());

    xcb_get_selection_owner_cookie_t cookie = xcb_get_selection_owner(connection,atom);

    xcb_generic_error_t *err = nullptr;
    xcb_get_selection_owner_reply_t *reply = xcb_get_selection_owner_reply(connection,cookie,&err);
    if(err != nullptr || reply == nullptr)
    {
        return false;
    }

    if(reply->owner == 0 || reply->owner != window)
    {
        return false;
    }

    return true;
}

QByteArray XcbUtils::getSettingPropValue()
{
    QByteArray array;
    xcb_atom_t atom =getAtom("_XSETTINGS_SETTINGS");
    xcb_list_properties_cookie_t cookies1 = xcb_list_properties(connection,window);
    xcb_list_properties_reply_t *reply = xcb_list_properties_reply(connection, cookies1, NULL);


       int len = xcb_list_properties_atoms_length(reply);
       xcb_atom_t *atoms = (xcb_atom_t *)xcb_list_properties_atoms(reply);
        QVector<xcb_atom_t> root_window_properties;
        root_window_properties.resize(len);
         memcpy(root_window_properties.data(), atoms, len * sizeof(xcb_atom_t));
//    xcb_get_property_cookie_t cookie = xcb_get_property(connection,0, window, atom,atom,0,10240);

//    xcb_generic_error_t *err = nullptr;
//    xcb_get_property_reply_t* reply = xcb_get_property_reply(connection,cookie,&err);
//    if(err != nullptr || reply == nullptr)
//    {
//        return array;
//    }
//    int length = xcb_get_property_value_length(reply);
//    if(length == 0)
//    {
//        return array;
//    }
//    void *c= xcb_get_property_value(reply);
//    char *value = static_cast<char*>(xcb_get_property_value(reply));

//    for(int i=0;i<length;i++)
//    {
//        array.push_back(*value);
//        value++;
//    }
    int offset = 0;
            QByteArray settings;
            while (1) {
                xcb_get_property_cookie_t cookie = xcb_get_property(connection,
                                                                    false,
                                                                    4194304,
                                                                    atom,
                                                                    atom,
                                                                    offset/4,
                                                                    8192);

                xcb_generic_error_t *error = nullptr;
                auto reply = xcb_get_property_reply(connection, cookie, &error);


                // 在窗口无效时，应当认为此native settings未初始化完成
                if (error && error->error_code == 3) {

                    return settings;
                }

                bool more = false;
                if (!reply)
                    return settings;

                const auto property_value_length = xcb_get_property_value_length(reply);
                settings.append(static_cast<const char *>(xcb_get_property_value(reply)), property_value_length);
                offset += property_value_length;
                more = reply->bytes_after != 0;
                free(reply);

                if (!more)
                    break;
            }

    return settings;
}

bool XcbUtils::changeSettingProp(QByteArray data)
{
    xcb_atom_t atom = getAtom("_XSETTINGS_SETTINGS");

    xcb_change_property_checked(connection,PropModeReplace,window,atom,atom,8,data.length(),data);
}

void XcbUtils::updateXResources(QVector<QPair<QString,QString>> xresourceInfos)
{
    QString datas = getXResources();

    if(datas.isEmpty())
    {
        xresourceInfos.push_back(qMakePair(QString("*customization"),QString("-color")));
    }else {
        QVector<QPair<QString,QString>>  infos = XcbUtils::unmarshalXResources(datas);

        for(auto info:infos)
        {
            bool bFound = false;
            for(auto xresourceInfo:xresourceInfos)
            {
                if(info.first == xresourceInfo.first)
                {
                    bFound = true;
                    break;
                }
            }

            if(!bFound)
            {
                xresourceInfos.push_back(info);
            }
        }
    }

    datas = XcbUtils::marshalXResources(xresourceInfos);

    XcbUtils::setXResources(const_cast<char*>(datas.toStdString().c_str()), datas.length());

}

char* XcbUtils::getXResources()
{
    Display *dpy = XOpenDisplay(nullptr);
    if(dpy==nullptr)
    {
        return nullptr;
    }

    char *res = XResourceManagerString(dpy);
    if(res == nullptr)
    {
        XCloseDisplay(dpy);
        return nullptr;
    }

    char *ret = strdup(res);
    XCloseDisplay(dpy);
    return ret;
}

int  XcbUtils::setXResources(char *data, unsigned long length)
{
    Display *dpy = XOpenDisplay(nullptr);
    if(dpy==nullptr)
    {
        return -1;
    }

    XChangeProperty(dpy, DefaultRootWindow(dpy), XA_RESOURCE_MANAGER, XA_STRING, 8,
                    PropModeReplace, reinterpret_cast<const unsigned char*>(data), length);

    XCloseDisplay(dpy);
    return 0;
}

QVector<QPair<QString, QString>> XcbUtils::unmarshalXResources(const QString &datas)
{
    QVector<QPair<QString, QString>> infos;

    QStringList lines = datas.split("\n");
    for(auto line : lines)
    {
        if(line.isEmpty())
        {
            continue;
        }

        QStringList arr = line.split(":\t");
        if(arr.size() != 2)
        {
            continue;
        }

        infos.push_back(qMakePair(arr[0],arr[1]));
    }

    return infos;
}

QString XcbUtils::marshalXResources(const QVector<QPair<QString, QString>> &infos)
{
    QString data;

    for(auto &info : infos)
    {
        data += info.first + ":\t" + info.second + "\n";
    }

    return data;
}
