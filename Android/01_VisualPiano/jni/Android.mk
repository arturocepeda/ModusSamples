LOCAL_PATH := $(call my-dir)

BUILD_CONFIG := Debug

include $(LOCAL_PATH)/../../GameEngine.mk
include $(LOCAL_PATH)/../../Modus.mk

include $(CLEAR_VARS)

LOCAL_MODULE           := VisualPiano
LOCAL_C_INCLUDES       := $(LOCAL_PATH) $(LOCAL_PATH)/../../../source/01_VisualPiano $(LOCAL_PATH)/../../../../GameEngine $(LOCAL_PATH)/../../../../Modus/include $(LOCAL_PATH)/../../../../Modus/soundgen/OpenSL
LOCAL_SRC_FILES        := Android.cpp AssetManager.cpp ../../../source/01_VisualPiano/StateSample.cpp ../../../../Modus/soundgen/OpenSL/mxsoundgenopensl.cpp
LOCAL_STATIC_LIBRARIES := Modus Tremor GameEngine stb pugixml cpufeatures
LOCAL_LDLIBS           := -llog -landroid -lGLESv2 -lOpenSLES
LOCAL_CFLAGS           := -std=c++11 -DLIB
LOCAL_ARM_MODE         := arm

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/cpufeatures)