#include "dconfinfos.h"
#include <QDebug>

DconfInfo::DconfInfo(QString dconfKey,QString xsKey,DconfValueType dconfType, int8_t xsType)
    :dconfKey(dconfKey)
    ,dconfType(dconfType)
    ,xsKey(xsKey)
    ,xsType(xsType)
{
}

void DconfInfo::setGsToXsFunc(ConverFun func)
{
    convertGsToXs = func;
}

void DconfInfo::setXsToGsFunc(ConverFun func)
{
    convertXsToGs = func;
}

XsValue DconfInfo::getValue(const DTK_CORE_NAMESPACE::DConfig& dconf)
{
    XsValue retVal;
    if(!dconf.isValid())
    {
        return retVal;
    }

    bool bOk=false;
    switch (dconfType) {
    case typeOfBool:
    case typeOfInt:
        int valInt;
        valInt = dconf.value(dconfKey).toInt(&bOk);
        if(!bOk)
        {
            return  retVal;
        }
        retVal = valInt;
        break;
    case typeOfString:
        retVal = dconf.value(dconfKey).toString();
        break;
    case typeOfDoublue:
        double valDouble;
        valDouble = dconf.value(dconfKey).toDouble(&bOk);
        if (!bOk) {
            return  retVal;
        }
        retVal = valDouble;
        break;
    default:
        return retVal;
    }

    if(convertGsToXs)
    {
        retVal = convertGsToXs(retVal);
    }

    return retVal;
}

bool DconfInfo::setValue(DTK_CORE_NAMESPACE::DConfig& dconf,XsValue& value)
{
    XsValue converValue = value;
    int* valInt ;
    QString* valString ;
    double* valDouble ;
    if(convertXsToGs)
    {
         converValue = convertXsToGs(value);
    }

    switch (dconfType) {
    case typeOfBool:
        valInt = std::get_if<int>(&converValue);
        if(valInt == nullptr)
        {
            return false;
        }

        if(*valInt == 1)
        {
            dconf.setValue(dconfKey,true);
        }else {
            dconf.setValue(dconfKey,false);
        }
        break;
    case typeOfInt:
        valInt = std::get_if<int>(&converValue);
        if(valInt == nullptr)
        {
            return false;
        }
        dconf.setValue(dconfKey,*valInt);
        break;
    case typeOfString:
        valString = std::get_if<QString>(&converValue);
        if(valString == nullptr)
        {
            return false;
        }

        dconf.setValue(dconfKey,*valString);
        break;
    case typeOfDoublue:
        valDouble = std::get_if<double>(&converValue);
        if(valDouble == nullptr)
        {
            return false;
        }

        dconf.setValue(dconfKey,*valDouble);
        break;
    }

    return true;
}

DconfInfo::ConverFun DconfInfo::getGsToXsFunc(ConverFun func)
{
    return convertGsToXs;
}

DconfInfo::ConverFun DconfInfo::getXsToGsFunc(ConverFun func)
{
    return convertXsToGs;
}

XsValue DconfInfo::convertStrToDouble(XsValue& value)
{
    QString *tempValue = std::get_if<QString>(&value);
    if(tempValue == nullptr)
    {
        return XsValue();
    }

    return  tempValue->toDouble();
}

XsValue DconfInfo::convertDoubleToStr(XsValue& value)
{
    double* tempValue = std::get_if<double>(&value);
    if(tempValue == nullptr)
    {
        return XsValue();
    }
    return  QString::number(*tempValue);
}

XsValue DconfInfo::convertStrToColor(XsValue& value)
{
    ColorValueInfo valueInfo;
    QString* tempValue = std::get_if<QString>(&value);
    if(tempValue == nullptr)
    {
        return XsValue();
    }
    QStringList valueArray = tempValue->split(",");
    if(valueArray.length() != 4)
    {
        return valueInfo;
    }

    for (int i = 0; i < colorSize; i++){
        valueInfo[i] = (uint16_t)((valueArray[i].toDouble()) / double(UINT16_MAX) * double(UINT8_MAX));
    }
    return valueInfo;
}

XsValue DconfInfo::convertColorToStr(XsValue& value)
{
    ColorValueInfo* tempValue = std::get_if<ColorValueInfo>(&value);
    if(tempValue == nullptr)
    {
        return XsValue();
    }

    uint16_t arr[4];
    for (int i = 0; i < colorSize; i++){
        arr[i] = (uint16_t)((double)((*tempValue)[i]) / (double)(UINT8_MAX) * (double)(UINT16_MAX));
    }
    return QString::asprintf("%d,%d,%d,%d",arr[0],arr[1],arr[2],arr[3]);
}

QString DconfInfo::getDconfKey()
{
    return dconfKey;
}

QString DconfInfo::getXsetKey()
{
    return xsKey;
}

int8_t DconfInfo::getKeyDType()
{
    return dconfType;
}

int8_t DconfInfo::getKeySType()
{
    if(xsType != HeadTypeInvalid)
    {
        return xsType;
    }

    if (dconfType == typeOfString || dconfType == typeOfDoublue) {
        return HeadTypeString;
    }else {
        return HeadTypeInteger;
    }
}

DconfInfos::DconfInfos()
    :dconfArray{QSharedPointer<DconfInfo>(new DconfInfo("Theme_Name","Net/ThemeName",typeOfString))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Icon_Theme_Name", "Net/IconThemeName",typeOfString))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Fallback_Icon_Theme", "Net/FallbackIconTheme",typeOfString))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Sound_Theme_Name", "Net/SoundThemeName",typeOfString))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Gtk_Theme_Name", "Gtk/ThemeName",typeOfString))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Gtk_Cursor_Theme_Name", "Gtk/CursorThemeName",typeOfString))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Gtk_Font_Name", "Gtk/FontName",typeOfString))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Gtk_Key_Theme_Name", "Gtk/KeyThemeName",typeOfString))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Gtk_Color_Palette", "Gtk/ColorPalette",typeOfString))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Gtk_Toolbar_Style", "Gtk/ToolbarStyle",typeOfString))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Gtk_Toolbar_Icon_Size", "Gtk/ToolbarIconSize",typeOfString))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Gtk_Tolor_Scheme", "Gtk/ColorScheme",typeOfString))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Gtk_Im_Preedit_Style", "Gtk/IMPreeditStyle",typeOfString))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Gtk_Im_Status_Style", "Gtk/IMStatusStyle",typeOfString))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Gtk_Im_Module", "Gtk/IMModule",typeOfString))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Gtk_Modules", "Gtk/Modules",typeOfString))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Gtk_Menubar_Accel", "Gtk/MenuBarAccel",typeOfString))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Xft_Hintstyle", "Xft/HintStyle",typeOfString))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Xft_Rgba", "Xft/RGBA",typeOfString))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Cursor_Blink_Time", "Net/CursorBlinkTime",typeOfInt))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Gtk_Cursor_Blink_Timeout", "Net/CursorBlinkTimeout",typeOfInt))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Double_Click_Time", "Net/DoubleClickTime",typeOfInt))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Double_Click_Distance", "Net/DoubleClickDistance",typeOfInt))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Dnd_Drag_Threshold", "Net/DndDragThreshold",typeOfInt))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Gtk_Cursor_Theme_Size", "Gtk/CursorThemeSize",typeOfInt))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Gtk_Timeout_Initial", "Gtk/TimeoutInitial",typeOfInt))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Gtk_Timeout_Repeat", "Gtk/TimeoutRepeat",typeOfInt))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Gtk_Recent_Files_Max_Age", "Gtk/RecentFilesMaxAge",typeOfInt))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Xft_Dpi", "Xft/DPI",typeOfInt))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Qursor_Blink", "Net/CursorBlink",typeOfBool))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Enable_Event_Sounds", "Net/EnableEventSounds",typeOfBool))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Enable_Input_Feedback_Sounds", "Net/EnableInputFeedbackSounds",typeOfBool))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Gtk_Can_Change_Accels", "Gtk/CanChangeAccels",typeOfBool))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Gtk_Menu_Images", "Gtk/MenuImages",typeOfBool))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Gtk_Button_Images", "Gtk/ButtonImages",typeOfBool))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Gtk_Enable_Animations", "Gtk/EnableAnimations",typeOfBool))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Gtk_Show_Input_Method_Menu", "Gtk/ShowInputMethodMenu",typeOfBool))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Gtk_Show_Unicode_Menu", "Gtk/ShowUnicodeMenu",typeOfBool))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Gtk_Auto_Mnemonics", "Gtk/AutoMnemonics",typeOfBool))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Gtk_Recent_Files_Enabled", "Gtk/RecentFilesEnabled",typeOfBool))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Gtk_Shell_Shows_App_Menu", "Gtk/ShellShowsAppMenu",typeOfBool))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Xft_Antialias", "Xft/Antialias",typeOfBool))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Xft_Hinting", "Xft/Hinting",typeOfBool))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Qt_Font_Name", "Qt/FontName",typeOfString))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Qt_Mono_Font_Name", "Qt/MonoFontName",typeOfString))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Dtk_Window_Radius", "DTK/WindowRadius",typeOfInt))
                ,QSharedPointer<DconfInfo>(new DconfInfo("Primary_Monitor_Name", "Gdk/PrimaryMonitorName",typeOfString))}

{
     QSharedPointer<DconfInfo>  qtActiveColor(new DconfInfo("Qt_Active_Color", "Qt/ActiveColor",typeOfString,HeadTypeColor));
     qtActiveColor->setGsToXsFunc(std::bind(&DconfInfo::convertStrToColor,qtActiveColor.get(),std::placeholders::_1));
     qtActiveColor->setXsToGsFunc(std::bind(&DconfInfo::convertColorToStr,qtActiveColor.get(),std::placeholders::_1));
     dconfArray.push_back(qtActiveColor);

     QSharedPointer<DconfInfo> qtFontPoint(new DconfInfo("Qt_Font_Point_Size", "Qt/FontPointSize",typeOfDoublue));
     qtFontPoint->setGsToXsFunc(std::bind(&DconfInfo::convertDoubleToStr,qtFontPoint.get(),std::placeholders::_1));
     qtFontPoint->setXsToGsFunc(std::bind(&DconfInfo::convertStrToDouble,qtFontPoint.get(),std::placeholders::_1));
     dconfArray.push_back(qtFontPoint);
}

QSharedPointer<DconfInfo> DconfInfos::getByDconfKey(const QString& dconfKey)
{
    for(auto item : dconfArray)
    {
        if(item->getDconfKey() == dconfKey)
        {
            return item;
        }
    }

    return nullptr;
}

QSharedPointer<DconfInfo> DconfInfos::getByXSKey(const QString& xsettingKey)
{
    for(auto item : dconfArray)
    {
        if(item->getXsetKey() == xsettingKey)
        {
            return item;
        }
    }

    return nullptr;
}

