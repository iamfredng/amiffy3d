LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

AMIFFY3D_PATH := ../../../../../

LOCAL_MODULE    := liblua
LOCAL_SRC_FILES := $(AMIFFY3D_PATH)/3rd/lib/android/$(TARGET_ARCH_ABI)/liblua.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL
AMIFFY3D_PATH := $(LOCAL_PATH)/../../../../../

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include \
	$(AMIFFY3D_PATH)/include \
	$(AMIFFY3D_PATH)/3rd/include

# Add your application source files here...
LOCAL_SRC_FILES := $(SDL_PATH)/src/main/android/SDL_android_main.c \
	$(AMIFFY3D_PATH)/src/lua_manager.cc \
	main.cc

LOCAL_SHARED_LIBRARIES := SDL2
LOCAL_STATIC_LIBRARIES := lua

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog -lEGL
LOCAL_CPPFLAGS := -std=c++11

include $(BUILD_SHARED_LIBRARY)
