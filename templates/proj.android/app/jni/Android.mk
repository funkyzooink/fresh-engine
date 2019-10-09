LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := MyGame_shared

LOCAL_MODULE_FILENAME := libMyGame

FILE_LIST := $(LOCAL_PATH)/hellocpp/main.cpp
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../../Classes/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../../Classes/3psw/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../../Classes/Characters/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../../Classes/GameConfig/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../../Classes/Objects/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/../../../Classes/Scenes/*.cpp)
LOCAL_SRC_FILES += $(FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../Classes
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../Classes/3psw
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../Classes/Characters
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../Classes/GameConfig
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../Classes/Objects
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../../../Classes/Scenes

# _COCOS_HEADER_ANDROID_BEGIN
# _COCOS_HEADER_ANDROID_END


LOCAL_STATIC_LIBRARIES := cc_static

# _COCOS_LIB_ANDROID_BEGIN
# _COCOS_LIB_ANDROID_END

include $(BUILD_SHARED_LIBRARY)

$(call import-module, cocos)

# _COCOS_LIB_IMPORT_ANDROID_BEGIN
# _COCOS_LIB_IMPORT_ANDROID_END
