
# Actuator drivers for both ardrone versions are conditionally included here
# The AT-command and RAW drivers are not interchangeble

ifeq ($(BOARD_TYPE), sdk)
  $(TARGET).CFLAGS += -DACTUATORS -DUSE_ACTUATORS_AT
  $(TARGET).srcs   += $(SRC_BOARD)/actuators_at.c
else ifeq ($(BOARD_TYPE), raw)
  $(TARGET).CFLAGS += -DACTUATORS
  $(TARGET).srcs   += $(SRC_BOARD)/actuators_ardrone2_raw.c
endif
