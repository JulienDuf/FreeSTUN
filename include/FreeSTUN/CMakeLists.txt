set(MODULE_NAME "FreeSTUN-headers")
set(MODULE_PREFIX "FREESTUN_HEADERS")

set(${MODULE_PREFIX}_HEADERS
        FreeStunConfig.h
        FreeStun.h
        FreeStunSocket.h
        FreeStunRequest.h
        FreeStunBuffer.h)

add_custom_target(${MODULE_NAME} SOURCES ${${MODULE_PREFIX}_HEADERS})