LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := ExamSheetScanner
LOCAL_SRC_FILES :=\
	EssBitmap.c\
	ExamSheetScannerUtil.c\
	ExamSheetScannerCore.c\
	lib_c.c
LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -llog
LOCAL_LDLIBS := -lz -llog

include $(BUILD_SHARED_LIBRARY)