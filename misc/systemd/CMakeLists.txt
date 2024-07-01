set(SYSTEMD_FILES
    dde-xsetting.service
)

install(FILES ${SYSTEMD_FILES} DESTINATION /usr/lib/systemd/user/)
