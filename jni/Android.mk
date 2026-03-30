LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := zamera_core_static
LOCAL_SRC_FILES := libzamera_core.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := hello
LOCAL_SRC_FILES := hello.c
LOCAL_STATIC_LIBRARIES := android_native_app_glue zamera_core_static
LOCAL_LDLIBS    := -llog -lGLESv2 -lEGL
include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
