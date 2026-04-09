include $(BL60X_SDK_PATH)/components/network/ble/ble_common.mk

COMPONENT_ADD_INCLUDEDIRS := .

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS := .
