LOCAL_PATH := $(call my-dir)


include $(CLEAR_VARS)
LOCAL_MODULE := gothook
LOCAL_SRC_FILES :=  main.c hook.c mylib.c
LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -llog -lEGL
include $(BUILD_SHARED_LIBRARY)

