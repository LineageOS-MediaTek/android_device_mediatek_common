LOCAL_PATH := $(call my-dir)

ifeq ($(TARGET_MEDIATEK_MT6737-COMMON),true)
include $(call all-makefiles-under,$(LOCAL_PATH))
endif
