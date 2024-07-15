#include "keyfile.h"
#include "locale.h"

#include <cstring>
#include <string>
#include <iostream>
#include <QDebug>

KeyFile::KeyFile(char separtor)
 : modified(false)
 , listSeparator(separtor)
{
}

KeyFile::~KeyFile()
{
    if (fp.isOpen()) {
        fp.close();
    }
}

bool KeyFile::getBool(const QString &section, const QString &key, bool defaultValue)
{
    if (mainKeyMap.find(section) == mainKeyMap.end())
        return false;

    QString valueStr = mainKeyMap[section][key];
    bool value = defaultValue;
    if (valueStr == "true")
        value = true;
    else if (valueStr == "false")
        value = false;

    return value;
}

QString KeyFile::getStr(const QString &section, const QString &key, QString defaultValue)
{
    if (mainKeyMap.find(section) == mainKeyMap.end())
        return defaultValue;

    QString valueStr = mainKeyMap[section][key];
    if (valueStr.isEmpty())
        valueStr = defaultValue;

    return valueStr;
}

bool KeyFile::containKey(const QString &section, const QString &key)
{
    if (mainKeyMap.find(section) == mainKeyMap.end())
        return false;

    return mainKeyMap[section].find(key) != mainKeyMap[section].end();
}

QStringList KeyFile::getStrList(const QString &section, const QString &key)
{
    QString value = getStr(section, key);

    return value.split(listSeparator);
}

// 修改keyfile内容
void KeyFile::setKey(const QString &section, const QString &key, const QString &value)
{
    if (mainKeyMap.find(section) == mainKeyMap.end())
    {
        mainKeyMap.insert(section, KeyMap());
    }

    mainKeyMap[section].insert(key, value);
}

bool KeyFile::deleteKey(const QString &section, const QString &key)
{
    if (mainKeyMap.find(section) == mainKeyMap.end())
    {
        return false;
    }
    mainKeyMap[section].remove(key);
    return false;
}

// 写入文件
bool KeyFile::saveToFile(const QString &filePath)
{
    qDebug() << filePath;
    QFile file(filePath);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return false;
    }

    for (const auto &im : mainKeyMap.toStdMap()) {
        const auto &keyMap = im.second;
        QString section = "[" + im.first + "]\n";
        qDebug() << section;
        file.write(section.toLatin1());
        for (const auto &ik : keyMap.toStdMap()) {
            QString kv = ik.first + "=" + ik.second + "\n";
            qDebug() << kv;
            file.write(kv.toLatin1());
        }
    }

    file.close();
    return true;
}

bool KeyFile::loadFile(const QString &filePath)
{
    mainKeyMap.clear();
    if (fp.isOpen()) {
        fp.close();
    }

    QString lastSection;
    fp.setFileName(filePath);

    if(!fp.open(QIODevice::ReadOnly))
    {
        return false;
    }

    QString line;
    while (!fp.atEnd()) {
        line=fp.readLine();
        // 移除行首空行
        line.replace(QRegExp("^ +"),"");
        if(line.front()=='#')
        {
            continue;
        }

        line.replace(QRegExp("\\t$"),"");
        line.replace(QRegExp("\\r$"),"");
        line.replace(QRegExp("\\n$"),"");

        int lPos = line.indexOf('[');
        int rPos = line.indexOf(']');
        if (lPos !=-1 && rPos !=-1 && rPos > lPos && lPos == 0 && rPos+1 == line.size()) {
            // 主键
            QString section = line.mid(lPos+1,line.size()-2);
            mainKeyMap.insert(section, KeyMap());
            lastSection = section;
        } else {
            int index = line.indexOf('=');
            if (index ==-1)
            {
                continue;
            }

            // 文件格式错误
            if (lastSection.isEmpty()){
                return false;
            }

            // 子键
            QString key = line.mid(0, index);
            QString value = line.mid(index + 1, line.length()-index-1);
            if(mainKeyMap.count(lastSection) == 1)
            {
                mainKeyMap[lastSection][key]=value;
            }
        }
    }
    fp.close();

    return true;
}

QStringList KeyFile::getMainKeys()
{
    QStringList mainKeys;
    for (const auto &iter : mainKeyMap.toStdMap())
        mainKeys.push_back(iter.first);

    return mainKeys;
}

void KeyFile::print()
{
    qDebug() << "sectionMap: ";
    for (auto sectionMap : mainKeyMap.toStdMap()) {

        KeyMap keyMap = sectionMap.second;
        qDebug() << sectionMap.first;
        for (auto iter : keyMap.toStdMap()) {
            qDebug() << iter.first << "=" << iter.second;
        }


    }
    qDebug() << "sectionMap: end";
}
