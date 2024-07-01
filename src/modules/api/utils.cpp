#include "utils.h"

#include <QDir>
#include <pwd.h>
#include <unistd.h>

Utils::Utils()
{

}


bool Utils::readString(QByteArray& array, QString& value, int length)
{
    if(array.length() < length )
    {
        return false;
    }

    value = array.left(length);

    array.remove(0,length);

    return true;
}

bool Utils::readSkip(QByteArray& array, int length)
{
    if(array.length()<length)
    {
        return false;
    }

    array.remove(0,length);

    return true;
}

bool Utils::writeString(QByteArray& array, const QString& value)
{
    for(auto iter:value)
    {
        array.push_back(iter.toLatin1());
    }
    return true;
}

bool Utils::writeSkip(QByteArray& array, int length)
{
    for(int i = 0;i<length;i++)
    {
        array.push_back(' ');
    }
    return true;
}

int Utils::getPad(int e)
{
    return (4 - (e % 4)) % 4;
}

QString Utils::GetUserConfigDir()
{
    // default $HOME/.config
    QString userConfigDir = getUserDir("XDG_CONFIG_HOME");

    if (userConfigDir.isEmpty()) {
        userConfigDir = getUserHomeDir();
        if (!userConfigDir.isEmpty()) {
            userConfigDir += "/.config";
        }

    }
    return userConfigDir;
}

QString Utils::getUserDir(const char *envName)
{
    const char *envDir = getenv(envName);
    if (envDir == nullptr) {
        return "";
    }

    if (!QDir::isAbsolutePath(envDir)) {
        return "";
    }

    return envDir;
}

QString Utils::getUserHomeDir()
{
    const char *dir = getenv("HOME");

    if (dir) {
        return dir;
    }

    struct passwd *user = getpwuid(getuid());
    if (user) {
        return user->pw_dir;
    }

    return "";
}

bool Utils::hasXsValue(const XsValue& value)
{
    if(!std::get_if<int>(&value) && !std::get_if<int>(&value)
            &&!std::get_if<int>(&value) && !std::get_if<int>(&value))
        {
        return false;
    }

    return true;
}
