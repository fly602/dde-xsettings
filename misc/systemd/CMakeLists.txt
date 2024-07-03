set(SYSTEMD_FILES
    dde-xsettings.service
)

install(FILES ${SYSTEMD_FILES} DESTINATION /usr/lib/systemd/user/)
