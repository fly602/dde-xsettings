#ifndef UTILS_H
#define UTILS_H

#include "../common/common.h"

#include <QByteArray>
#include <QString>
#include <QDataStream>
#include <QtEndian>

class Utils
{
public:
    Utils();
    template<typename Value>
    static  bool readInteger(QByteArray& array, Value& value)
    {
        int size = sizeof (Value);
        if(array.size() < size)
        {
            return false;
        }

        QByteArray tempArray(array,size);
        std::reverse(tempArray.begin(),tempArray.end());
        QDataStream ds(tempArray);
        // Since the size you're trying to read appears to be 2 bytes
        ds >> value;
        array = array.remove(0,size);

        return true;
    }

    template<typename Value>
    static  bool writeInteger(QByteArray& array, const Value& value)
    {
        int size = sizeof (Value)/8;
        char* p = reinterpret_cast<char*>(const_cast<Value*>(&value));

        for(int i= size-1;i >= 0;i--)
        {
            array.push_back(p[i]);
        }
        return true;
    }
    static  bool readString(QByteArray& array, QString& value, int length);
    static  bool readSkip(QByteArray& array, int length);
    static  bool writeString(QByteArray& array, const QString& value);
    static  bool writeSkip(QByteArray& array, int length);
    static  int  getPad(int e);
    static QString GetUserConfigDir();
    static QString getUserDir(const char *envName);
    static QString getUserHomeDir();
    static bool  hasXsValue(const XsValue& value);
};

#endif // UTILS_H
