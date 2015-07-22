LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../prebuild)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../../cocos2d/cocos)

LOCAL_MODULE := cocos2dcpp_shared

LOCAL_MODULE_FILENAME := libcocos2dcpp

LOCAL_SRC_FILES := hellocpp/main.cpp \
                  ../../Classes/AppDelegate.cpp \
                   ../../Classes/CCStreamVideoPlayer.cpp \
				   ../../Classes/CCUdpReceiver.cpp \
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
					$(LOCAL_PATH)/../../Classes/nanovg \
					$(LOCAL_PATH)/../../prebuild/ffmpeg/arm/include
					
LOCAL_SHARED_LIBRARIES := postproc-prebuilt
LOCAL_SHARED_LIBRARIES += avformat-prebuilt
LOCAL_SHARED_LIBRARIES += avcodec-prebuilt 
LOCAL_SHARED_LIBRARIES += avdevice-prebuilt
LOCAL_SHARED_LIBRARIES += avfilter-prebuilt						
LOCAL_SHARED_LIBRARIES += avutil-prebuilt
LOCAL_SHARED_LIBRARIES += swresample-prebuilt 
LOCAL_SHARED_LIBRARIES += swscale-prebuilt

LOCAL_STATIC_LIBRARIES := 
LOCAL_STATIC_LIBRARIES += x264-prebuilt
LOCAL_STATIC_LIBRARIES += cocos2dx_static 

include $(BUILD_SHARED_LIBRARY)

$(call import-module,ffmpeg)
$(call import-module,.)
