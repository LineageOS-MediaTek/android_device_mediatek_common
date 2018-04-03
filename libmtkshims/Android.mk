LOCAL_PATH := $(call my-dir)

# mtk log symbols (xlog)
include $(CLEAR_VARS)
LOCAL_SRC_FILES := xlog.cpp
LOCAL_SHARED_LIBRARIES := liblog
LOCAL_MODULE := libmtkshim_log
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)

# libc symbols
include $(CLEAR_VARS)
LOCAL_SRC_FILES := bionic.cpp
LOCAL_MODULE := libmtkshim_bionic
LOCAL_SHARED_LIBRARIES := libc
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)

# netutils symbols
include $(CLEAR_VARS)
LOCAL_SRC_FILES := netutils.c
LOCAL_SHARED_LIBRARIES := liblog libcutils
LOCAL_MODULE := libmtkshim_ifc
LOCAL_C_INCLUDES += system/core/libnetutils/include
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)


# audio symbols
include $(CLEAR_VARS)
LOCAL_SRC_FILES := audio.cpp
LOCAL_MODULE := libmtkshim_audio
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)


# ui symbols
include $(CLEAR_VARS)
LOCAL_SRC_FILES := GraphicBuffer.cpp
LOCAL_SHARED_LIBRARIES := libui libgui
LOCAL_MODULE := libmtkshim_ui
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)


# omx symbols
include $(CLEAR_VARS)
LOCAL_SRC_FILES := omx.cpp
LOCAL_MODULE := libmtkshim_omx
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)

# gps symbols
include $(CLEAR_VARS)
LOCAL_SRC_FILES := agps/icu53.c agps/ssl.c
LOCAL_SHARED_LIBRARIES := libicuuc libssl
LOCAL_MODULE := libmtkshim_gps
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)

