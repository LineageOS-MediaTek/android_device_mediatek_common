# Configs
PRODUCT_COPY_FILES += \
    vendor/mad/prebuilt/etc/apns-conf.xml:system/etc/apns-conf.xml \
    vendor/mad/prebuilt/etc/ecc_list.xml:system/etc/ecc_list.xml \
    vendor/mad/prebuilt/etc/spn-conf.xml:system/etc/spn-conf.xml

# Messaging
PRODUCT_PACKAGES += \
    messaging \
    Stk