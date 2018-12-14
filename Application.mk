APP_ABI := armeabi armeabi-v7a

APP_PLATFORM := android-14

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
APP_PROJECT_PATH := $(call my-dir)/../
APP_MODULES      := gothook inlinehook memdump inject
 
test-root-dir:=$(APP_PROJECT_PATH)jni/