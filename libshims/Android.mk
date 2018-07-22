LOCAL_PATH := $(call my-dir)

# /system/vendor/lib*/libui_ext.so:/system/vendor/lib/libMtkOmxVenc.so ( _ZN7android13GraphicBufferC1Ejjij )
include $(CLEAR_VARS)
LOCAL_SRC_FILES := GraphicBuffer.cpp
LOCAL_SHARED_LIBRARIES := libui
LOCAL_MODULE := libshim_ui
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)

# /system/vendor/lib*/libgui_ext.so ( _ZN7android11IDumpTunnel11asInterfaceERKNS_2spINS_7IBinderEEE )
include $(CLEAR_VARS)
LOCAL_SRC_FILES := gui.cpp
LOCAL_SHARED_LIBRARIES := libui
LOCAL_MODULE := libshim_gui
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)

# /system/lib*/libandroid_runtime.so ( _ZN7android14CameraMetadata14getTagFromNameEPKcPKNS_19VendorTagDescriptorEPj )
include $(CLEAR_VARS)
LOCAL_SRC_FILES := misc.cpp
LOCAL_MODULE := libshim_misc
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)

# /system/vendor/bin/thermal ( ifc_set_txq_state ) : /system/vendor/lib*/mtk-ril.so ( ifc_ipv6_trigger_rs )
include $(CLEAR_VARS)
LOCAL_SRC_FILES := netutils.c
LOCAL_SHARED_LIBRARIES := liblog libcutils
LOCAL_MODULE := libshim_ifc
LOCAL_C_INCLUDES += system/core/libnetutils/include
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)
