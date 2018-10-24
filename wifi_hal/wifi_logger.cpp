#include <stdint.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>
#include <linux/rtnetlink.h>
#include <netpacket/packet.h>
#include <linux/filter.h>
#include <linux/errqueue.h>

#include <linux/pkt_sched.h>
#include <netlink/object-api.h>
#include <netlink/netlink.h>
#include <netlink/socket.h>
#include <netlink-types.h>

#include "nl80211_copy.h"
#include "sync.h"

#define LOG_TAG  "WifiHAL"

#include <log/log.h>

#include "wifi_hal.h"
#include "common.h"
#include "cpp_bindings.h"

typedef enum {
    LOGGER_START_LOGGING = ANDROID_NL80211_SUBCMD_DEBUG_RANGE_START,
    LOGGER_TRIGGER_MEM_DUMP,
    LOGGER_GET_MEM_DUMP,
    LOGGER_GET_VER,
    LOGGER_GET_RING_STATUS,
    LOGGER_GET_RING_DATA,
    LOGGER_GET_FEATURE,
    LOGGER_RESET_LOGGING,
} DEBUG_SUB_COMMAND;

typedef enum {
    LOGGER_ATTRIBUTE_DRIVER_VER,
    LOGGER_ATTRIBUTE_FW_VER,
    LOGGER_ATTRIBUTE_RING_ID,
    LOGGER_ATTRIBUTE_RING_NAME,
    LOGGER_ATTRIBUTE_RING_FLAGS,
    LOGGER_ATTRIBUTE_LOG_LEVEL,
    LOGGER_ATTRIBUTE_LOG_TIME_INTVAL,
    LOGGER_ATTRIBUTE_LOG_MIN_DATA_SIZE,
    LOGGER_ATTRIBUTE_FW_DUMP_LEN,
    LOGGER_ATTRIBUTE_FW_DUMP_DATA,
    // LOGGER_ATTRIBUTE_FW_ERR_CODE,
    LOGGER_ATTRIBUTE_RING_DATA,
    LOGGER_ATTRIBUTE_RING_STATUS,
    LOGGER_ATTRIBUTE_RING_NUM,
} LOGGER_ATTRIBUTE;

typedef enum {
    DEBUG_OFF = 0,
    DEBUG_NORMAL,
    DEBUG_VERBOSE,
    DEBUG_VERY,
    DEBUG_VERY_VERY,
} LOGGER_LEVEL;

typedef enum {
    GET_FW_VER,
    GET_DRV_VER,
    GET_RING_DATA,
    GET_RING_STATUS,
    GET_FEATURE,
    START_RING_LOG,
} GetCmdType;


///////////////////////////////////////////////////////////////////////////////
class DebugCommand : public WifiCommand
{
    char *mBuff;
    int *mBuffSize;
    u32 *mNumRings;
    wifi_ring_buffer_status *mStatus;
    unsigned int *mSupport;
    u32 mVerboseLevel;
    u32 mFlags;
    u32 mMaxIntervalSec;
    u32 mMinDataSize;
    char *mRingName;
    GetCmdType mType;

public:

    // constructor for get version
    DebugCommand(wifi_interface_handle iface, char *buffer, int *buffer_size,
            GetCmdType cmdType)
        : WifiCommand(iface, 0), mBuff(buffer), mBuffSize(buffer_size), mType(cmdType)
    {
        memset(mBuff, 0, *mBuffSize);
    }

    // constructor for ring data
    DebugCommand(wifi_interface_handle iface, char *ring_name, GetCmdType cmdType)
        : WifiCommand(iface, 0), mRingName(ring_name), mType(cmdType)
    { }

    // constructor for ring status
    DebugCommand(wifi_interface_handle iface, u32 *num_rings,
            wifi_ring_buffer_status *status, GetCmdType cmdType)
        : WifiCommand(iface, 0), mNumRings(num_rings), mStatus(status), mType(cmdType)
    {
        memset(mStatus, 0, sizeof(wifi_ring_buffer_status) * (*mNumRings));
    }

    // constructor for feature set
    DebugCommand(wifi_interface_handle iface, unsigned int *support, GetCmdType cmdType)
        : WifiCommand(iface, 0), mSupport(support), mType(cmdType)
    { }

    // constructor for ring params
    DebugCommand(wifi_interface_handle iface, u32 verbose_level, u32 flags,
            u32 max_interval_sec, u32 min_data_size, char *ring_name, GetCmdType cmdType)
        : WifiCommand(iface, 0), mVerboseLevel(verbose_level), mFlags(flags),
        mMaxIntervalSec(max_interval_sec), mMinDataSize(min_data_size),
        mRingName(ring_name), mType(cmdType)
    { }

    int createRingRequest(WifiRequest& request) {
        int result = request.create(GOOGLE_OUI, LOGGER_START_LOGGING);
        if (result != WIFI_SUCCESS) {
            ALOGE("Failed to create start ring logger request; result = %d", result);
            return result;
        }

        nlattr *data = request.attr_start(NL80211_ATTR_VENDOR_DATA);

        result = request.put_u32(LOGGER_ATTRIBUTE_LOG_LEVEL, mVerboseLevel);
        if (result != WIFI_SUCCESS) {
            ALOGE("Failed to put log level; result = %d", result);
            return result;
        }
        result = request.put_u32(LOGGER_ATTRIBUTE_RING_FLAGS, mFlags);
        if (result != WIFI_SUCCESS) {
            ALOGE("Failed to put ring flags; result = %d", result);
            return result;
        }
        result = request.put_u32(LOGGER_ATTRIBUTE_LOG_TIME_INTVAL, mMaxIntervalSec);
        if (result != WIFI_SUCCESS) {
            ALOGE("Failed to put log time interval; result = %d", result);
            return result;
        }
        result = request.put_u32(LOGGER_ATTRIBUTE_LOG_MIN_DATA_SIZE, mMinDataSize);
        if (result != WIFI_SUCCESS) {
            ALOGE("Failed to put min data size; result = %d", result);
            return result;
        }
        result = request.put_string(LOGGER_ATTRIBUTE_RING_NAME, mRingName);
        if (result != WIFI_SUCCESS) {
            ALOGE("Failed to put ringbuffer name; result = %d", result);
            return result;
        }
        request.attr_end(data);

        return WIFI_SUCCESS;
    }

    int createRequest(WifiRequest &request) {
        int result;

        switch (mType) {
            case GET_FW_VER:
            {
                result = request.create(GOOGLE_OUI, LOGGER_GET_VER);
                if (result != WIFI_SUCCESS) {
                    ALOGE("Failed to create get fw version request; result = %d", result);
                    return result;
                }

                nlattr *data = request.attr_start(NL80211_ATTR_VENDOR_DATA);

                // Driver expecting only attribute type, passing mbuff as data with
                // length 0 to avoid undefined state
                result = request.put(LOGGER_ATTRIBUTE_FW_VER, mBuff, 0);
                if (result != WIFI_SUCCESS) {
                    ALOGE("Failed to put get fw version request; result = %d", result);
                    return result;
                }
                request.attr_end(data);
                break;
            }

            case GET_DRV_VER:
            {
                result = request.create(GOOGLE_OUI, LOGGER_GET_VER);
                if (result != WIFI_SUCCESS) {
                    ALOGE("Failed to create get drv version request; result = %d", result);
                    return result;
                }

                nlattr *data = request.attr_start(NL80211_ATTR_VENDOR_DATA);

                // Driver expecting only attribute type, passing mbuff as data with
                // length 0 to avoid undefined state
                result = request.put(LOGGER_ATTRIBUTE_DRIVER_VER, mBuff, 0);

                if (result != WIFI_SUCCESS) {
                    ALOGE("Failed to put get drv version request; result = %d", result);
                    return result;
                }
                request.attr_end(data);
                break;
            }

            case GET_RING_DATA:
            {
                result = request.create(GOOGLE_OUI, LOGGER_GET_RING_DATA);
                if (result != WIFI_SUCCESS) {
                    ALOGE("Failed to create get ring data request; result = %d", result);
                    return result;
                }

                nlattr *data = request.attr_start(NL80211_ATTR_VENDOR_DATA);
                result = request.put_string(LOGGER_ATTRIBUTE_RING_NAME, mRingName);
                if (result != WIFI_SUCCESS) {
                    ALOGE("Failed to put ring data request; result = %d", result);
                    return result;
                }
                request.attr_end(data);
                break;
            }

            case GET_RING_STATUS:
            {
                result = request.create(GOOGLE_OUI, LOGGER_GET_RING_STATUS);
                if (result != WIFI_SUCCESS) {
                    ALOGE("Failed to create get ring status request; result = %d", result);
                    return result;
                }
                break;
            }

            case GET_FEATURE:
            {
                result = request.create(GOOGLE_OUI, LOGGER_GET_FEATURE);
                if (result != WIFI_SUCCESS) {
                    ALOGE("Failed to create get feature request; result = %d", result);
                    return result;
                }
                break;
            }

            case START_RING_LOG:
                result = createRingRequest(request);
                break;

            default:
                ALOGE("Unknown Debug command");
                result = WIFI_ERROR_UNKNOWN;
        }
        return result;
    }

    int start() {
        ALOGD("Start debug command");
        WifiRequest request(familyId(), ifaceId());
        int result = createRequest(request);
        if (result != WIFI_SUCCESS) {
            ALOGE("Failed to create debug request; result = %d", result);
            return result;
        }

        result = requestResponse(request);
        if (result != WIFI_SUCCESS) {
            ALOGE("Failed to register debug response; result = %d", result);
        }
        return result;
    }

    virtual int handleResponse(WifiEvent& reply) {
        ALOGD("In DebugCommand::handleResponse");

        if (reply.get_cmd() != NL80211_CMD_VENDOR) {
            ALOGD("Ignoring reply with cmd = %d", reply.get_cmd());
            return NL_SKIP;
        }

        switch (mType) {
            case GET_DRV_VER:
            case GET_FW_VER:
            {
                void *data = reply.get_vendor_data();
                int len = reply.get_vendor_data_len();

                ALOGD("len = %d, expected len = %d", len, *mBuffSize);
                memcpy(mBuff, data, min(len, *mBuffSize));
                if (*mBuffSize < len)
                    return NL_SKIP;
                *mBuffSize = len;
                break;
            }

            case START_RING_LOG:
            case GET_RING_DATA:
                break;

            case GET_RING_STATUS:
            {
                nlattr *vendor_data = reply.get_attribute(NL80211_ATTR_VENDOR_DATA);
                int len = reply.get_vendor_data_len();
                wifi_ring_buffer_status *status(mStatus);

                if (vendor_data == NULL || len == 0) {
                    ALOGE("No Debug data found");
                    return NL_SKIP;
                }

                nl_iterator it(vendor_data);
                if (it.get_type() == LOGGER_ATTRIBUTE_RING_NUM) {
                    unsigned int num_rings = it.get_u32();
                    if (*mNumRings < num_rings) {
                        ALOGE("Not enough status buffers provided, available: %d required: %d",
                                *mNumRings, num_rings);
                    } else {
                        *mNumRings = num_rings;
                    }
                } else {
                    ALOGE("Unknown attribute: %d expecting %d",
                            it.get_type(), LOGGER_ATTRIBUTE_RING_NUM);
                    return NL_SKIP;
                }

                it.next();
                for (unsigned int i = 0; it.has_next() && i < *mNumRings; it.next()) {
                    if (it.get_type() == LOGGER_ATTRIBUTE_RING_STATUS) {
                        memcpy(status, it.get_data(), sizeof(wifi_ring_buffer_status));
                        i++;
                        status++;
                    } else {
                        ALOGW("Ignoring invalid attribute type = %d, size = %d",
                                it.get_type(), it.get_len());
                    }
                }
                break;
            }

            case GET_FEATURE:
            {
                void *data = reply.get_vendor_data();
                int len = reply.get_vendor_data_len();

                ALOGD("len = %d, expected len = %d", len, sizeof(unsigned int));
                memcpy(mSupport, data, sizeof(unsigned int));
                break;
            }

            default:
                ALOGW("Unknown Debug command");
        }
        return NL_OK;
    }

    virtual int handleEvent(WifiEvent& event) {
        /* NO events! */
        return NL_SKIP;
    }
};

/* API to collect a firmware version string */
wifi_error wifi_get_firmware_version(wifi_interface_handle iface, char *buffer,
        int buffer_size)
{
    if (buffer && (buffer_size > 0)) {
        DebugCommand *cmd = new DebugCommand(iface, buffer, &buffer_size, GET_FW_VER);
        return (wifi_error)cmd->start();
    } else {
        ALOGE("FW version buffer NULL");
        return  WIFI_ERROR_INVALID_ARGS;
    }
}

/* API to collect a driver version string */
wifi_error wifi_get_driver_version(wifi_interface_handle iface, char *buffer, int buffer_size)
{
    if (buffer && (buffer_size > 0)) {
        DebugCommand *cmd = new DebugCommand(iface, buffer, &buffer_size, GET_DRV_VER);
        return (wifi_error)cmd->start();
    } else {
        ALOGE("Driver version buffer NULL");
        return  WIFI_ERROR_INVALID_ARGS;
    }
}

/* API to collect driver records */
wifi_error wifi_get_ring_data(wifi_interface_handle iface, char *ring_name)
{
    DebugCommand *cmd = new DebugCommand(iface, ring_name, GET_RING_DATA);
    return (wifi_error)cmd->start();
}

/* API to get the status of all ring buffers supported by driver */
wifi_error wifi_get_ring_buffers_status(wifi_interface_handle iface,
        u32 *num_rings, wifi_ring_buffer_status *status)
{
    if (status && num_rings) {
        DebugCommand *cmd = new DebugCommand(iface, num_rings, status, GET_RING_STATUS);
        return (wifi_error)cmd->start();
    } else {
        ALOGE("Ring status buffer NULL");
        return  WIFI_ERROR_INVALID_ARGS;
    }
}

/* API to get supportable feature */
wifi_error wifi_get_logger_supported_feature_set(wifi_interface_handle iface,
        unsigned int *support)
{
    if (support) {
        DebugCommand *cmd = new DebugCommand(iface, support, GET_FEATURE);
        return (wifi_error)cmd->start();
    } else {
        ALOGE("Get support buffer NULL");
        return  WIFI_ERROR_INVALID_ARGS;
    }
}

wifi_error wifi_start_logging(wifi_interface_handle iface, u32 verbose_level,
        u32 flags, u32 max_interval_sec, u32 min_data_size, char *ring_name)
{
    if (ring_name) {
        DebugCommand *cmd = new DebugCommand(iface, verbose_level, flags,
                max_interval_sec, min_data_size, ring_name, START_RING_LOG);
        return (wifi_error)cmd->start();
    } else {
        ALOGE("Ring name NULL");
        return  WIFI_ERROR_INVALID_ARGS;
    }
}


///////////////////////////////////////////////////////////////////////////////
class SetLogHandler : public WifiCommand
{
    wifi_ring_buffer_data_handler mHandler;

public:
    SetLogHandler(wifi_interface_handle iface, int id, wifi_ring_buffer_data_handler handler)
        : WifiCommand(iface, id), mHandler(handler)
    { }
    SetLogHandler(wifi_interface_handle iface, int id)
        : WifiCommand(iface, id)
    { }

    int start() {
        ALOGD("Register log handler");
        registerVendorHandler(GOOGLE_OUI, GOOGLE_DEBUG_RING_EVENT);
        return WIFI_SUCCESS;
    }

    virtual int cancel() {
        /* Send a command to driver to stop generating logging events */
        ALOGD("Reset event handler");

        WifiRequest request(familyId(), ifaceId());
        int result = request.create(GOOGLE_OUI, LOGGER_RESET_LOGGING);

        if (result != WIFI_SUCCESS) {
            ALOGE("failed to create reset request; result = %d", result);
            return result;
        }

        result = requestResponse(request);
        if (result != WIFI_SUCCESS) {
            ALOGE("failed to request reset; result = %d", result);
            return result;
        }

        /* unregister event handler */
        unregisterVendorHandler(GOOGLE_OUI, GOOGLE_DEBUG_RING_EVENT);
        ALOGD("Success to reset event handler");
        return WIFI_SUCCESS;
    }

    virtual int handleEvent(WifiEvent& event) {
        char *buffer = NULL;
        int buffer_size = 0;

        ALOGD("In SetLogHandler::handleEvent");
        nlattr *vendor_data = event.get_attribute(NL80211_ATTR_VENDOR_DATA);
        int len = event.get_vendor_data_len();
        int event_id = event.get_vendor_subcmd();
        ALOGI("Got Logger event: %d", event_id);

        if (vendor_data == NULL || len == 0) {
            ALOGE("No Debug data found");
            return NL_SKIP;
        }

        if(event_id == GOOGLE_DEBUG_RING_EVENT) {
            wifi_ring_buffer_status status;
            memset(&status, 0, sizeof(status));

            for (nl_iterator it(vendor_data); it.has_next(); it.next()) {
                if (it.get_type() == LOGGER_ATTRIBUTE_RING_STATUS) {
                    memcpy(&status, it.get_data(), sizeof(status));
                } else if (it.get_type() == LOGGER_ATTRIBUTE_RING_DATA) {
                    buffer_size = it.get_len();
                    buffer = (char *)it.get_data();
                } else {
                    ALOGW("Ignoring invalid attribute type = %d, size = %d",
                            it.get_type(), it.get_len());
                }
            }

            ALOGI("Retrieved Debug data");
            if (mHandler.on_ring_buffer_data) {
                (*mHandler.on_ring_buffer_data)((char *)status.name, buffer, buffer_size,
                        &status);
            }
        } else {
            ALOGE("Unknown Event");
            return NL_SKIP;
        }
        return NL_OK;
    }
};

wifi_error wifi_set_log_handler(wifi_request_id id, wifi_interface_handle iface,
        wifi_ring_buffer_data_handler handler)
{
    wifi_handle handle = getWifiHandle(iface);
    SetLogHandler *cmd = new SetLogHandler(iface, id, handler);

    ALOGI("Logger start, handle = %p", handle);
    if (cmd) {
        wifi_register_cmd(handle, id, cmd);
        return (wifi_error)cmd->start();
    } else {
        ALOGD("Out of memory");
        return WIFI_ERROR_OUT_OF_MEMORY;
    }
}

wifi_error wifi_reset_log_handler(wifi_request_id id, wifi_interface_handle iface)
{
    wifi_handle handle = getWifiHandle(iface);
    SetLogHandler *cmd = new SetLogHandler(iface, id);

    ALOGI("Logger reset, handle = %p", handle);
    if (cmd) {
        cmd->cancel();
        cmd->releaseRef();
        return WIFI_SUCCESS;
    }
    return WIFI_ERROR_INVALID_ARGS;
}

///////////////////////////////////////////////////////////////////////////////
class SetAlertHandler : public WifiCommand
{
    wifi_alert_handler mHandler;
    int mBuffSize;
    char *mBuff;
    int mErrCode;

public:
    SetAlertHandler(wifi_interface_handle iface, int id, wifi_alert_handler handler)
        : WifiCommand(iface, id), mHandler(handler), mBuffSize(0), mBuff(NULL), mErrCode(0)
    { }

    int start() {
        ALOGD("Start Alerting");
        registerVendorHandler(GOOGLE_OUI, GOOGLE_DEBUG_MEM_DUMP_EVENT);
        return WIFI_SUCCESS;
    }

    virtual int handleResponse(WifiEvent& reply) {
        ALOGD("In SetAlertHandler::handleResponse");

        if (reply.get_cmd() != NL80211_CMD_VENDOR) {
            ALOGD("Ignoring reply with cmd = %d", reply.get_cmd());
            return NL_SKIP;
        }

        nlattr *vendor_data = reply.get_attribute(NL80211_ATTR_VENDOR_DATA);
        int len = reply.get_vendor_data_len();

        ALOGD("len = %d", len);
        if (vendor_data == NULL || len == 0) {
            ALOGE("no vendor data in memory dump response; ignoring it");
            return NL_SKIP;
        }

        for (nl_iterator it(vendor_data); it.has_next(); it.next()) {
            if (it.get_type() == LOGGER_ATTRIBUTE_FW_DUMP_DATA) {
                ALOGI("Initiating alert callback");
                if (mHandler.on_alert) {
                    (*mHandler.on_alert)(id(), mBuff, mBuffSize, mErrCode);
                }
                if (mBuff) {
                    free(mBuff);
                    mBuff = NULL;
                }
            }
        }
        return NL_OK;
    }

    virtual int handleEvent(WifiEvent& event) {
        wifi_ring_buffer_id ring_id;
        char *buffer = NULL;
        int buffer_size = 0;


        nlattr *vendor_data = event.get_attribute(NL80211_ATTR_VENDOR_DATA);
        int len = event.get_vendor_data_len();
        int event_id = event.get_vendor_subcmd();
        ALOGI("Got event: %d", event_id);

        if (vendor_data == NULL || len == 0) {
            ALOGE("No Debug data found");
            return NL_SKIP;
        }

        if (event_id == GOOGLE_DEBUG_MEM_DUMP_EVENT) {
            for (nl_iterator it(vendor_data); it.has_next(); it.next()) {
                if (it.get_type() == LOGGER_ATTRIBUTE_FW_DUMP_LEN) {
                    mBuffSize = it.get_u32();
                } else if (it.get_type() == LOGGER_ATTRIBUTE_RING_DATA) {
                    buffer_size = it.get_len();
                    buffer = (char *)it.get_data();
            /*
                } else if (it.get_type() == LOGGER_ATTRIBUTE_FW_ERR_CODE) {
                    mErrCode = it.get_u32();
            */
                } else {
                    ALOGW("Ignoring invalid attribute type = %d, size = %d",
                            it.get_type(), it.get_len());
                }
            }
            if (mBuffSize) {
                ALOGD("dump size: %d meta data size: %d", mBuffSize, buffer_size);
                if (mBuff) free(mBuff);
                mBuff = (char *)malloc(mBuffSize + buffer_size);
                if (!mBuff) {
                    ALOGE("Buffer allocation failed");
                    return NL_SKIP;
                }
                memcpy(mBuff, buffer, buffer_size);

                WifiRequest request(familyId(), ifaceId());
                int result = request.create(GOOGLE_OUI, LOGGER_GET_MEM_DUMP);
                if (result != WIFI_SUCCESS) {
                    ALOGE("Failed to create get memory dump request; result = %d", result);
                    free(mBuff);
                    return NL_SKIP;
                }
                nlattr *data = request.attr_start(NL80211_ATTR_VENDOR_DATA);
                result = request.put_u32(LOGGER_ATTRIBUTE_FW_DUMP_LEN, mBuffSize);
                if (result != WIFI_SUCCESS) {
                    ALOGE("Failed to put get memory dump request; result = %d", result);
                    return result;
                }

                result = request.put_u64(LOGGER_ATTRIBUTE_FW_DUMP_DATA,
                         (uint64_t)(mBuff+buffer_size));
                if (result != WIFI_SUCCESS) {
                    ALOGE("Failed to put get memory dump request; result = %d", result);
                    return result;
                }
                request.attr_end(data);
                mBuffSize += buffer_size;

                result = requestResponse(request);

                if (result != WIFI_SUCCESS) {
                    ALOGE("Failed to register get momory dump response; result = %d", result);
                }
            } else {
                ALOGE("dump event missing dump length attribute");
                return NL_SKIP;
            }
        }
        return NL_OK;
    }
};

wifi_error wifi_set_alert_handler(wifi_request_id id, wifi_interface_handle iface,
        wifi_alert_handler handler)
{
    wifi_handle handle = getWifiHandle(iface);
    SetAlertHandler *cmd = new SetAlertHandler(iface, id, handler);
    ALOGI("Alert start, handle = %p", handle);

    wifi_register_cmd(handle, id, cmd);
    return (wifi_error)cmd->start();
}


///////////////////////////////////////////////////////////////////////////////
class MemoryDumpCommand: public WifiCommand
{
    wifi_firmware_memory_dump_handler mHandler;
    int mBuffSize;
    char *mBuff;

public:
    MemoryDumpCommand(wifi_interface_handle iface, wifi_firmware_memory_dump_handler handler)
        : WifiCommand(iface, 0), mHandler(handler), mBuffSize(0), mBuff(NULL)
    { }

    int start() {
        ALOGD("Start memory dump command");
        WifiRequest request(familyId(), ifaceId());

        int result = request.create(GOOGLE_OUI, LOGGER_TRIGGER_MEM_DUMP);
        if (result != WIFI_SUCCESS) {
            ALOGE("Failed to create trigger fw memory dump request; result = %d", result);
            return result;
        }

        result = requestResponse(request);
        if (result != WIFI_SUCCESS) {
            ALOGE("Failed to register trigger memory dump response; result = %d", result);
        }
        return result;
    }

    virtual int handleResponse(WifiEvent& reply) {
        ALOGD("In MemoryDumpCommand::handleResponse");

        if (reply.get_cmd() != NL80211_CMD_VENDOR) {
            ALOGD("Ignoring reply with cmd = %d", reply.get_cmd());
            return NL_SKIP;
        }

        nlattr *vendor_data = reply.get_attribute(NL80211_ATTR_VENDOR_DATA);
        int len = reply.get_vendor_data_len();

        ALOGD("len = %d", len);
        if (vendor_data == NULL || len == 0) {
            ALOGE("no vendor data in memory dump response; ignoring it");
            return NL_SKIP;
        }

        for (nl_iterator it(vendor_data); it.has_next(); it.next()) {
            if (it.get_type() == LOGGER_ATTRIBUTE_FW_DUMP_LEN) {
                mBuffSize = it.get_u32();

                if (mBuff)
                    free(mBuff);
                mBuff = (char *)malloc(mBuffSize);
                if (!mBuff) {
                    ALOGE("Buffer allocation failed");
                    return NL_SKIP;
                }
                WifiRequest request(familyId(), ifaceId());
                int result = request.create(GOOGLE_OUI, LOGGER_GET_MEM_DUMP);
                if (result != WIFI_SUCCESS) {
                    ALOGE("Failed to create get memory dump request; result = %d", result);
                    free(mBuff);
                    return NL_SKIP;
                }

                nlattr *data = request.attr_start(NL80211_ATTR_VENDOR_DATA);
                result = request.put_u32(LOGGER_ATTRIBUTE_FW_DUMP_LEN, mBuffSize);
                if (result != WIFI_SUCCESS) {
                    ALOGE("Failed to put get memory dump request; result = %d", result);
                    return result;
                }

                result = request.put_u64(LOGGER_ATTRIBUTE_FW_DUMP_DATA, (uint64_t)mBuff);
                if (result != WIFI_SUCCESS) {
                    ALOGE("Failed to put get memory dump request; result = %d", result);
                    return result;
                }
                request.attr_end(data);

                result = requestResponse(request);
                if (result != WIFI_SUCCESS) {
                    ALOGE("Failed to register get momory dump response; result = %d", result);
                }
            } else if (it.get_type() == LOGGER_ATTRIBUTE_FW_DUMP_DATA) {
                ALOGI("Initiating memory dump callback");
                if (mHandler.on_firmware_memory_dump) {
                    (*mHandler.on_firmware_memory_dump)(mBuff, mBuffSize);
                }
                if (mBuff) {
                    free(mBuff);
                    mBuff = NULL;
                }
            } else {
                ALOGW("Ignoring invalid attribute type = %d, size = %d",
                        it.get_type(), it.get_len());
            }
        }
        return NL_OK;
    }

    virtual int handleEvent(WifiEvent& event) {
        /* NO events! */
        return NL_SKIP;
    }
};

/* API to collect a firmware memory dump for a given iface */
wifi_error wifi_get_firmware_memory_dump( wifi_interface_handle iface,
        wifi_firmware_memory_dump_handler handler)
{
    MemoryDumpCommand *cmd = new MemoryDumpCommand(iface, handler);
    return (wifi_error)cmd->start();
}

