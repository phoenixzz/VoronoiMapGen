

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := sgp_jni
LOCAL_SRC_FILES := \
  ../../../SGPLibraryCode/modules/sgp_core/sgp_core.cpp\

ifeq ($(CONFIG),Debug)
  LOCAL_CPPFLAGS += -fsigned-char -fexceptions -frtti -g -I "../../SGPLibraryCode" -O0 -D "SGP_ANDROID=1" -D "SGP_ANDROID_API_VERSION=8" -D "SGP_ANDROID_ACTIVITY_CLASSNAME=com_yourcompany_sgpengine_SGPEngine" -D SGP_ANDROID_ACTIVITY_CLASSPATH=\"com/yourcompany/sgpengine/SGPEngine\" -D "DEBUG=1" -D "_DEBUG=1" -D "SGP_ANDROID_7F0E4A25=1"
  LOCAL_LDLIBS := -llog -lGLESv2
else
  LOCAL_CPPFLAGS += -fsigned-char -fexceptions -frtti -I "../../SGPLibraryCode" -Os -D "SGP_ANDROID=1" -D "SGP_ANDROID_API_VERSION=8" -D "SGP_ANDROID_ACTIVITY_CLASSNAME=com_yourcompany_sgpengine_SGPEngine" -D SGP_ANDROID_ACTIVITY_CLASSPATH=\"com/yourcompany/sgpengine/SGPEngine\" -D "NDEBUG=1" -D "SGP_ANDROID_7F0E4A25=1"
  LOCAL_LDLIBS := -llog -lGLESv2
endif

include $(BUILD_SHARED_LIBRARY)
