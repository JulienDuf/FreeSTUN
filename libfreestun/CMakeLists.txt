set(MODULE_NAME "freestun")
set(MODULE_PREFIX "LIBFREESTUN")

set(${MODULE_PREFIX}_HEADERS "")

set(${MODULE_PREFIX}_SOURCES
        FreeStun.c
        FreeStunSocket.c
        FreeStunBuffer.c
        FreeStunRequest.c
        FreeStunAttribute.c)

add_library(${MODULE_NAME} STATIC ${${MODULE_PREFIX}_SOURCES} ${${MODULE_PREFIX}_HEADERS})

set_target_properties(${MODULE_NAME} PROPERTIES OUTPUT_NAME "freestun")
set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER "FreeSTUN/lib")