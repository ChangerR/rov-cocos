LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/cocos)

LOCAL_MODULE := cocos2dcpp_shared

LOCAL_MODULE_FILENAME := libcocos2dcpp

LOCAL_SRC_FILES := hellocpp/main.cpp \
                   ../../Classes/AppDelegate.cpp \
                   ../../Classes/CCMjpegVideoViewr.cpp \
				   ../../Classes/RovScene.cpp \
				   ../../Classes/CRovLoginLayer.cpp \
				   ../../Classes/RovControler.cpp \
				   ../../Classes/CCHttpStream.cpp \
				   ../../Classes/CCMjpegStreamReceiver.cpp \
					../../Classes/CCAsyncHttpClient.cpp \
					../../Classes/CColorConvert.cpp \
					../../Classes/CRovStatus.cpp \
					../../Classes/nanovg/nanovg.c \
					../../Classes/CUITabMenu.cpp \
					../../Classes/CUIJoystick.cpp \
					../../Classes/CUIScrollBar.cpp \
					../../Classes/CCaptureNode.cpp
					
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes \
					$(LOCAL_PATH)/../../Classes/nanovg

LOCAL_STATIC_LIBRARIES := cocos2dx_static

include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)
