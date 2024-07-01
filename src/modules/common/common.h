#ifndef COMMON_H
#define COMMON_H
#include <QObject>
#include <variant>

#define gsKeyScaleFactor        "scale-factor"
#define gsKeyWindowScale        "window-scale"
#define gsKeyXftDpi             "xft-dpi"
#define gsKeyGtkCursorThemeSize "gtk-cursor-theme-size"
#define gsKeyIndividualScaling  "individual-scaling"
#define qtThemeSection                "Theme"
#define	qtThemeKeyScreenScaleFactors  "ScreenScaleFactors"
#define	qtThemeKeyScaleFactor         "ScaleFactor"
#define	qtThemeKeyScaleLogicalDpi     "ScaleLogicalDpi"

#define WRAPSCHEMA              "com.deepin.wrap.gnome.desktop.interface"

struct ItemHeader{
  uint8_t   type;
  uint16_t  length;
  QString   name;
  uint32_t  lastChangeSerial;
};

struct ColorValueInfo{
  uint16_t  red;
  uint16_t  green;
  uint16_t  blue;
  uint16_t  alpha;
};
using XsValue = std::variant<int,double,QString,ColorValueInfo>;

enum DconfValueType{
    typeOfBool=1,
    typeOfInt,
    typeOfString,
    typeOfDoublue
};

struct XsSetting{
    uint8_t type;
    QString prop;
    XsValue value;
};

const uint HeadTypeInvalid  =-1;
const uint HeadTypeInteger  =0;
const uint HeadTypeString   =1;
const uint HeadTypeColor    =2;



#endif // COMMON_H
