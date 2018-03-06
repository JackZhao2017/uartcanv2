root_path := $(call my-dir)


LOCAL_PATH := $(root_path)

include $(CLEAR_VARS)


LOCAL_MODULE := uartcan

LOCAL_SRC_FILES :=UartCan.cpp UartCanUtil/*cpp UartCanSend/*cpp  UartCanSend/Encode/*cpp UartCanReceive/UartCanRead/*cpp   UartCanReceive/UartCanParse/*cpp   UartCanReceive/*cpp




LOCAL_C_INCLUDES :=$(LOCAL_PATH)/UartCanUtil
LOCAL_C_INCLUDES +=$(LOCAL_PATH)/UartCanSend\
                  $(LOCAL_PATH)/UartCanReceive\
                  $(LOCAL_PATH)/UartCanReceive/UartCanRead\
                  $(LOCAL_PATH)/UartCanReceive/UartCanParse\
		  $(LOCAL_PATH)/UartCanSend/Encode

include $(BUILD_STATIC_LIBRARY)





