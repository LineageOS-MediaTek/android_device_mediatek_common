# Common overlays
DEVICE_PACKAGE_OVERLAYS += vendor/mad/overlay

# Shim libraries
PRODUCT_PACKAGES += \
    libmtkshim_log \
    libmtkshim_audio \
    libmtkshim_ui \
    libmtkshim_omx \
    libmtkshim_gps

# Display
PRODUCT_PACKAGES += \
    libion

# GPS
PRODUCT_COPY_FILES += \
    vendor/mad/prebuilt/etc/agps_profiles_conf2.xml:system/etc/agps_profiles_conf2.xml

# Chromium
PRODUCT_PACKAGES += \
    Chromium

# OmniStyle
PRODUCT_PACKAGES += \
    OmniStyle

# madCamera
PRODUCT_PACKAGES += \
    madCamera

# madLauncher
PRODUCT_PACKAGES += \
    madLauncher

# madGallery
PRODUCT_PACKAGES += \
    madGallery

# Include explicitly to work around Facelock issues
PRODUCT_PACKAGES += \
    libprotobuf-cpp-full

# LockClock
PRODUCT_PACKAGES += \
	LockClock

# FMRadio
PRODUCT_PACKAGES += \
    libfmjni \
    FMRadio

# Filesystem management tools
PRODUCT_PACKAGES += \
    e2fsck \
    fsck.f2fs \
    mkfs.f2fs \
    make_ext4fs

# exFAT
PRODUCT_PACKAGES += \
    mount.exfat \
    fsck.exfat \
    mkfs.exfat

# NTFS
PRODUCT_PACKAGES += \
    fsck.ntfs \
    mkfs.ntfs \
    mount.ntfs

# USB
PRODUCT_PACKAGES += \
    com.android.future.usb.accessory

# Charger
PRODUCT_PACKAGES += \
    charger_res_images \
    mad_charger_res_images

# WallpaperPicker
PRODUCT_PACKAGES += \
    WallpaperPicker

# Sensor Calibration
PRODUCT_PACKAGES += \
    libem_sensor_jni

# SchedulePowerOnOff
PRODUCT_PACKAGES += \
SchedulePowerOnOff

# ThemeInterfacer
PRODUCT_PACKAGES += \
    ThemeInterfacer

# Eleven
PRODUCT_PACKAGES += \
	Eleven

# Bootanimation
PRODUCT_COPY_FILES += \
    vendor/mad/prebuilt/bootanimation/bootanimation.zip:system/media/bootanimation.zip

PRODUCT_BUILD_PROP_OVERRIDES += BUILD_UTC_DATE=0

# madOS Versioning
ANDROID_VERSION = 7.1.1
MADOS_VERSION = 1.0

ifndef MADOS_BUILD_TYPE
    MADOS_BUILD_TYPE := OFFICIAL
endif

MADOS_MOD_VERSION := madOS-$(MADOS_VERSION)-$(shell date -u +%Y%m%d)-$(MADOS_BUILD_TYPE)

PRODUCT_PROPERTY_OVERRIDES += \
  ro.mados.version=$(MADOS_VERSION) \
  ro.mados.releasetype=$(MADOS_BUILD_TYPE) \
  ro.modversion=$(MADOS_MOD_VERSION)

MADOS_DISPLAY_VERSION := madOS-$(MADOS_VERSION)-$(MADOS_BUILD_TYPE)

PRODUCT_PROPERTY_OVERRIDES += \
  ro.mados.display.version=$(MADOS_DISPLAY_VERSION)

# include other configs
include vendor/mad/config/permissions.mk
include vendor/mad/config/media.mk
include vendor/mad/config/wifi.mk
include vendor/mad/config/telephony.mk
include vendor/mad/config/google_override.mk
