LOCAL_PATH := $(call my-dir)

# mtk log symbols (xlog)
ifeq ($(TARGET_INCLUDE_XLOG_SYMBOLS),true)
    include $(CLEAR_VARS)

    LOCAL_SRC_FILES := xlog.cpp
    LOCAL_SHARED_LIBRARIES := liblog
    LOCAL_MODULE := libmtkshim_log
    LOCAL_MODULE_TAGS := optional

    include $(BUILD_SHARED_LIBRARY)
endif

# libc symbols
ifeq ($(TARGET_INCLUDE_LIBC_SYMBOLS),true)
    include $(CLEAR_VARS)

    LOCAL_SRC_FILES := bionic.cpp
    LOCAL_MODULE := libmtkshim_bionic
    LOCAL_MODULE_TAGS := optional

    include $(BUILD_SHARED_LIBRARY)
endif

# netutils symbols
ifeq ($(TARGET_INCLUDE_NETUTILS_SYMBOLS),true)
    include $(CLEAR_VARS)

    LOCAL_SRC_FILES := netutils.c
    LOCAL_MODULE := libmtkshim_ifc
    LOCAL_C_INCLUDES += system/core/libnetutils/include
    LOCAL_MODULE_TAGS := optional

    include $(BUILD_SHARED_LIBRARY)
endif

# audio symbols
ifeq ($(TARGET_INCLUDE_AUDIO_SYMBOLS),true)
    include $(CLEAR_VARS)

    LOCAL_SRC_FILES := audio.cpp
    LOCAL_MODULE := libmtkshim_audio
    LOCAL_MODULE_TAGS := optional

    include $(BUILD_SHARED_LIBRARY)
endif

# ui symbols
ifeq ($(TARGET_INCLUDE_UI_SYMBOLS),true)
    include $(CLEAR_VARS)

    LOCAL_SRC_FILES := GraphicBuffer.cpp
    LOCAL_SHARED_LIBRARIES := libui libgui
    LOCAL_MODULE := libmtkshim_ui
    LOCAL_MODULE_TAGS := optional

    include $(BUILD_SHARED_LIBRARY)
endif

# omx symbols
ifeq ($(TARGET_INCLUDE_OMX_SYMBOLS),true)
    include $(CLEAR_VARS)

    LOCAL_SRC_FILES := omx.cpp
    LOCAL_MODULE := libmtkshim_omx
    LOCAL_MODULE_TAGS := optional

    include $(BUILD_SHARED_LIBRARY)
endif

# gps symbols
ifeq ($(TARGET_INCLUDE_GPS_SYMBOLS),true)
    include $(CLEAR_VARS)

    LOCAL_SRC_FILES := \
        agps/icu53.c \
        agps/ssl.c

    LOCAL_SHARED_LIBRARIES := libicuuc libssl
    LOCAL_MODULE := libmtkshim_gps
    LOCAL_MODULE_TAGS := optional

    include $(BUILD_SHARED_LIBRARY)
endif
