ap.CFLAGS += -DUSE_MAPPING

ap.CFLAGS += -DMAPPING_TYPE_H=\"subsystems/mapping/mapping_test.h\"
$(TARGET).srcs += $(SRC_SUBSYSTEMS)/mapping/mapping_test.c

$(TARGET).srcs += $(SRC_SUBSYSTEMS)/mapping/mapping_navigation.c
