LOCAL_PATH := $(call my-dir)

ifeq ($(TARGET_MEDIATEK_COMMON),true)
include $(call all-makefiles-under,$(LOCAL_PATH))
endif
