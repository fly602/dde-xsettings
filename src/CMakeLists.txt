# SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: LGPL-3.0-or-later
set(BIN_NAME "dde-xsettings")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Core DBus Concurrent)
find_package(DtkCore REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(XCB REQUIRED IMPORTED_TARGET xcb)
pkg_check_modules(X11 REQUIRED IMPORTED_TARGET x11)
pkg_check_modules(QGSettings REQUIRED IMPORTED_TARGET gsettings-qt)

pkg_check_modules(Openssl REQUIRED IMPORTED_TARGET openssl)

qt5_add_dbus_adaptor(ADAPTER_SOURCES
        ../dbus/org.deepin.dde.XSettings.xml
                     impl/xsettings1.h
                     XSettings1)
set(SRCS
    ./main.cpp
    ./impl/xsettings1.h
    ./impl/xsettings1.cpp
    ../dbus/types/scaleFactors.h
    ../dbus/types/scaleFactors.cpp
    ../dbus/types/arrayOfColor.h
    ../dbus/types/arrayOfColor.cpp
    ./impl/xsettingsmanager.h
    ./impl/xsettingsmanager.cpp
    ./impl/scalefactorhelper.h
    ./impl/scalefactorhelper.cpp
    ./modules/api/xcbutils.h
    ./modules/api/xcbutils.cpp
    ./modules/api/keyfile.h
    ./modules/api/keyfile.cpp
    ./impl/xsdatainfo.h
    ./impl/xsdatainfo.cpp
    ./modules/api/utils.h
    ./modules/api/utils.cpp
    ./impl/dconfinfos.h
    ./impl/dconfinfos.cpp
    ./modules/common/common.h
)

add_executable(${BIN_NAME}
    ${SRCS}
    ${ADAPTER_SOURCES}
)

target_link_libraries(${BIN_NAME}
    PkgConfig::Openssl
    Qt5::Core
    Qt5::Concurrent
    Qt5::DBus
    PkgConfig::XCB
    PkgConfig::X11
    PkgConfig::QGSettings
    ${DtkCore_LIBRARIES}
    )

install(TARGETS ${BIN_NAME} DESTINATION bin)
