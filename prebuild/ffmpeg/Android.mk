LOCAL_PATH := $(call my-dir)  

include $(CLEAR_VARS)  
LOCAL_MODULE := libavcodec-prebuilt  
LOCAL_MODULE_FILENAME := libavcodec-56
LOCAL_SRC_FILES := arm/lib/libavcodec-56.so
include $(PREBUILT_SHARED_LIBRARY)  

include $(CLEAR_VARS)  
LOCAL_MODULE := avdevice-prebuilt  
LOCAL_MODULE_FILENAME := libavdevice-56
LOCAL_SRC_FILES := arm/lib/libavdevice-56.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)  
LOCAL_MODULE := avfilter-prebuilt  
LOCAL_MODULE_FILENAME := libavfilter-5
LOCAL_SRC_FILES := arm/lib/libavfilter-5.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)  
LOCAL_MODULE := avformat-prebuilt 
LOCAL_MODULE_FILENAME := libavformat-56
LOCAL_SRC_FILES := arm/lib/libavformat-56.so 
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)  
LOCAL_MODULE := avutil-prebuilt  
LOCAL_MODULE_FILENAME := libavutil-54
LOCAL_SRC_FILES := arm/lib/libavutil-54.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)  
LOCAL_MODULE := swresample-prebuilt  
LOCAL_MODULE_FILENAME := libswresample-1
LOCAL_SRC_FILES := arm/lib/libswresample-1.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)  
LOCAL_MODULE := swscale-prebuilt  
LOCAL_MODULE_FILENAME := libswscale-3
LOCAL_SRC_FILES := arm/lib/libswscale-3.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)  
LOCAL_MODULE := postproc-prebuilt  
LOCAL_MODULE_FILENAME := libpostproc-53
LOCAL_SRC_FILES := arm/lib/libpostproc-53.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)  
LOCAL_MODULE := x264-prebuilt  
LOCAL_MODULE_FILENAME := libx264
LOCAL_SRC_FILES := arm/lib/libx264.a
include $(PREBUILT_STATIC_LIBRARY)

