set(DBUS_SERVICE_FILES
    org.deepin.dde.XSettings.service
)

install(FILES ${DBUS_SERVICE_FILES} DESTINATION /usr/share/dbus-1/services/)
