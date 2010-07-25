

ifeq ($(MODEM_UART), Uart0)
ap.CFLAGS += -DUSE_UART0 -DUART0_BAUD=B$(MODEM_BAUD)
else
ap.CFLAGS += -DUSE_UART1 -DUART1_BAUD=B$(MODEM_BAUD)
endif



ap.CFLAGS += -DDOWNLINK -DDOWNLINK_FBW_DEVICE=$(MODEM_UART) -DDOWNLINK_AP_DEVICE=$(MODEM_UART) -DPPRZ_UART=$(MODEM_UART)
ap.CFLAGS += -DDOWNLINK_TRANSPORT=PprzTransport -DDATALINK=PPRZ
ap.srcs += $(SRC_FIXEDWING)/downlink.c $(SRC_FIXEDWING)/datalink.c $(SRC_FIXEDWING)/pprz_transport.c

