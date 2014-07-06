###################################################################################################

include $(CLEAR_VARS)
LOCAL_MODULE := Modus
LOCAL_SRC_FILES := ../../lib/$(BUILD_CONFIG)/libModus.a
include $(PREBUILT_STATIC_LIBRARY)

###################################################################################################

include $(CLEAR_VARS)
LOCAL_MODULE := Tremor
LOCAL_SRC_FILES := ../../lib/$(BUILD_CONFIG)/libTremor.a
include $(PREBUILT_STATIC_LIBRARY)

###################################################################################################