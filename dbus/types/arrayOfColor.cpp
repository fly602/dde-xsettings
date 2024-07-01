#include "arrayOfColor.h"
void registerArrayOfColorMetaType()
{
    qRegisterMetaType<ArrayOfColor>("ArrayOfColor");
    qDBusRegisterMetaType<ArrayOfColor>();
}
