
ifndef JSBSIM_INC
JSBSIM_ROOT = /opt/jsbsim
JSBSIM_INC = $(JSBSIM_ROOT)/include/JSBSim
JSBSIM_LIB = $(JSBSIM_ROOT)/lib
endif

jsbsim.ARCHDIR = $(ARCH)

# external libraries
jsbsim.CFLAGS = -I$(SIMDIR) -I/usr/include `pkg-config glib-2.0 --cflags`
jsbsim.LDFLAGS += `pkg-config glib-2.0 --libs` -lm -lpcre -lglibivy -L/usr/lib

# use the paparazzi-jsbsim package if it is installed, otherwise look for JSBsim under /opt/jsbsim
ifndef JSBSIM_PKG
JSBSIM_PKG = $(shell pkg-config JSBSim --exists && echo 'yes')
endif
ifeq ($(JSBSIM_PKG), yes)
	jsbsim.CFLAGS  += `pkg-config JSBSim --cflags`
	jsbsim.LDFLAGS += `pkg-config JSBSim --libs`
else
	JSBSIM_PKG = no
	jsbsim.CFLAGS  += -I$(JSBSIM_INC)
	jsbsim.LDFLAGS += -L$(JSBSIM_LIB) -lJSBSim
endif

jsbsim.CFLAGS += -DSITL -DAP -DFBW -DRADIO_CONTROL -DINTER_MCU -DDOWNLINK -DDOWNLINK_TRANSPORT=IvyTransport -DINFRARED -DNAV -DLED -DWIND_INFO
jsbsim.srcs = $(SRC_ARCH)/jsbsim_hw.c $(SRC_ARCH)/jsbsim_gps.c $(SRC_ARCH)/jsbsim_ir.c $(SRC_ARCH)/jsbsim_transport.c $(SRC_ARCH)/ivy_transport.c
jsbsim.srcs += latlong.c radio_control.c downlink.c commands.c gps.c inter_mcu.c infrared.c fw_h_ctl.c fw_v_ctl.c nav.c estimator.c sys_time.c main_fbw.c main_ap.c datalink.c
jsbsim.srcs += $(SIMDIR)/sim_ac_jsbsim.c
# Choose in your airframe file type of airframe
# jsbsim.srcs += $(SIMDIR)/sim_ac_fw.c
# jsbsim.srcs += $(SIMDIR)/sim_ac_booz.c
