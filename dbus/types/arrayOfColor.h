#ifndef ARRAYOFCOLOR_H
#define ARRAYOFCOLOR_H

#include <QDBusMetaType>

typedef QList<quint16> ArrayOfColor;

void registerArrayOfColorMetaType();

#endif // ARRAYOFCOLOR_H
