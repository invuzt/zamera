APP_ABI := arm64-v8a
APP_PLATFORM := android-23

LOCAL_CFLAGS += -Os -ffunction-sections -fdata-sections
LOCAL_LDFLAGS += -Wl,--gc-sections

