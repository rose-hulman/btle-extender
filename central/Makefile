###
# BTLE central project Makefile
#
# Oringially copied from Nordic:
# nRF5_SDK_11.0.0-2.alpha_bc3f6a0/examples/ble_central/ble_app_hrs_c/
#
# Parameters:
#	BUILD_TYPE	Debug			Default
#			Release
###

# Include Nordic specific definitons
include ../common/rules/nrf5x_defs.mak

###
# The executable name that is being built.
###
TARGET_NAME	= btle_central

###
# The build type: Debug or Release (case sensetive)
# This will be the artifact (.bin, .hex, .map, .o, etc.) output directory
###
BUILD_TYPE	?= Debug

###
# BTLE central source
# Note: These I_PATHS need to preceed the $(NORDIC_SDK_ROOT)/components/drivers_nrf/config
# so that project specific definitions have priority over the defaults.
###
SDK_SRC += $(NORDIC_SDK_ROOT)/examples/ble_central/ble_app_hrs_c/main.c
I_PATHS += $(NORDIC_SDK_ROOT)/examples/ble_central/ble_app_hrs_c/config
I_PATHS += $(NORDIC_SDK_ROOT)/examples/ble_central/ble_app_hrs_c/config/ble_app_hrs_c_$(SOFT_DEVICE_LOWER)_$(BOARD_TYPE_LOWER)

###
# Segger RTT for console debug output
###
SDK_SRC += $(NORDIC_SDK_ROOT)/components/drivers_ext/segger_rtt/RTT_Syscalls_GCC.c
SDK_SRC += $(NORDIC_SDK_ROOT)/components/drivers_ext/segger_rtt/SEGGER_RTT.c
SDK_SRC += $(NORDIC_SDK_ROOT)/components/drivers_ext/segger_rtt/SEGGER_RTT_printf.c

###
# Nordic console (uart and RTT) logging
###
SDK_SRC += $(NORDIC_SDK_ROOT)/components/libraries/fifo/app_fifo.c
SDK_SRC += $(NORDIC_SDK_ROOT)/components/libraries/uart/app_uart_fifo.c
SDK_SRC += $(NORDIC_SDK_ROOT)/components/libraries/uart/retarget.c
SDK_SRC += $(NORDIC_SDK_ROOT)/components/drivers_nrf/uart/nrf_drv_uart.c

###
# Nordic common logging services
###
SDK_SRC += $(NORDIC_SDK_ROOT)/components/libraries/trace/app_trace.c
SDK_SRC += $(NORDIC_SDK_ROOT)/components/libraries/util/nrf_log.c

###
#
###
SDK_SRC += $(NORDIC_SDK_ROOT)/components/libraries/util/app_error.c
SDK_SRC += $(NORDIC_SDK_ROOT)/components/libraries/timer/app_timer.c
SDK_SRC += $(NORDIC_SDK_ROOT)/components/libraries/util/nrf_assert.c
SDK_SRC += $(NORDIC_SDK_ROOT)/components/drivers_nrf/delay/nrf_delay.c
SDK_SRC += $(NORDIC_SDK_ROOT)/components/drivers_nrf/common/nrf_drv_common.c
SDK_SRC += $(NORDIC_SDK_ROOT)/components/drivers_nrf/gpiote/nrf_drv_gpiote.c
SDK_SRC += $(NORDIC_SDK_ROOT)/components/drivers_nrf/pstorage/pstorage.c

###
# Buttons and LEDs
###
SDK_SRC += $(NORDIC_SDK_ROOT)/components/libraries/button/app_button.c
SDK_SRC += $(NORDIC_SDK_ROOT)/examples/bsp/bsp.c
SDK_SRC += $(NORDIC_SDK_ROOT)/exmaples/bsp/bsp_btn_ble.c

###
# Advertising and connections
###
SDK_SRC += $(NORDIC_SDK_ROOT)/components/ble/common/ble_advdata.c
SDK_SRC += $(NORDIC_SDK_ROOT)/components/ble/ble_advertising/ble_advertising.c
SDK_SRC += $(NORDIC_SDK_ROOT)/components/ble/common/ble_conn_params.c

###
# GATT services
###
SDK_SRC += $(NORDIC_SDK_ROOT)/components/ble/ble_db_discovery/ble_db_discovery.c
SDK_SRC += $(NORDIC_SDK_ROOT)/components/ble/ble_services/ble_hrs_c/ble_hrs_c.c
SDK_SRC += $(NORDIC_SDK_ROOT)/components/ble/ble_services/ble_bas_c/ble_bas_c.c

###
# Nordic device and softdevice
###
SDK_SRC += $(NORDIC_SDK_ROOT)/components/ble/common/ble_srv_common.c
SDK_SRC += $(NORDIC_SDK_ROOT)/components/ble/device_manager/device_manager_central.c
SDK_SRC += $(NORDIC_SDK_ROOT)/components/toolchain/system_$(NORDIC_DEVICE_LOWER).c
SDK_SRC += $(NORDIC_SDK_ROOT)/components/softdevice/common/softdevice_handler/softdevice_handler.c

I_PATHS += $(NORDIC_SDK_ROOT)/components/ble/ble_advertising
I_PATHS += $(NORDIC_SDK_ROOT)/components/device
I_PATHS += $(NORDIC_SDK_ROOT)/components/softdevice/$(SOFT_DEVICE_LOWER)/headers
I_PATHS += $(NORDIC_SDK_ROOT)/components/toolchain/gcc
I_PATHS += $(NORDIC_SDK_ROOT)/components/ble/ble_radio_notification
I_PATHS += $(NORDIC_SDK_ROOT)/components/ble/ble_services/ble_dfu
I_PATHS += $(NORDIC_SDK_ROOT)/components/drivers_nrf/ble_flash
I_PATHS += $(NORDIC_SDK_ROOT)/components/drivers_nrf/hal
I_PATHS += $(NORDIC_SDK_ROOT)/components/libraries/bootloader_dfu
I_PATHS += $(NORDIC_SDK_ROOT)/components/drivers_nrf/config
I_PATHS += $(NORDIC_SDK_ROOT)/components/drivers_nrf/gpiote
I_PATHS += $(NORDIC_SDK_ROOT)/components/drivers_nrf/uart
I_PATHS += $(NORDIC_SDK_ROOT)/examples/bsp
I_PATHS += $(NORDIC_SDK_ROOT)/components/libraries/button
I_PATHS += $(NORDIC_SDK_ROOT)/components/libraries/hci

#I_PATHS += $(NORDIC_SDK_ROOT)/components/ble/ble_services/ble_bas_c
#I_PATHS += $(NORDIC_SDK_ROOT)/components/ble/peer_manager
#I_PATHS += $(NORDIC_SDK_ROOT)/components/libraries/uart
#I_PATHS += $(NORDIC_SDK_ROOT)/components/ble/ble_services/ble_rscs
#I_PATHS += $(NORDIC_SDK_ROOT)/components/ble/ble_services/ble_hrs_c
#I_PATHS += $(NORDIC_SDK_ROOT)/components/ble/ble_services/ble_rscs_c

###
# Nordic specific source files for compiling DFU into an application.
###
ifeq ($(NORDIC_DFU_SUPPORT), 1)
SDK_SRC += $(NORDIC_SDK_ROOT)/components/libraries/bootloader_dfu/bootloader_util.c
SDK_SRC += $(NORDIC_SDK_ROOT)/components/libraries/bootloader_dfu/dfu_app_handler.c
SDK_SRC += $(NORDIC_SDK_ROOT)/components/ble/ble_services/ble_dfu/ble_dfu.c
endif

###
# ARM GCC assembly initialization code.
# Sets up the stack, initializes .data segment values to their C language initialized values.
###
GCC_STARTUP_S += $(NORDIC_SDK_ROOT)/components/toolchain/gcc/gcc_startup_$(NORDIC_DEVICE_LOWER).s

C_SOURCE_FILES	= $(SDK_SRC)
S_SOURCE_FILES	= $(GCC_STARTUP_S)

###
# The linked definition (.ld) script is found in the project sub-directories;
# differentiated by softdevice and board type.
# The linker script include nrf5x_common.ld (where x=1,2) file is located in
# $(NORDIC_SDK_ROOT)/components/toolchain/gcc and also needs included in the path.
###
LD_PATHS += $(NORDIC_SDK_ROOT)/examples/ble_central/ble_app_hrs_c/$(BOARD_TYPE_LOWER)/$(SOFT_DEVICE_LOWER)/armgcc
LD_PATHS += $(NORDIC_SDK_ROOT)/components/toolchain/gcc

LINKER_SCRIPT = ble_app_hrs_c_gcc_$(NORDIC_DEVICE_LOWER).ld

.PHONY:  all relink clean info

# Building all targets
all: 	$(BUILD_TYPE)				\
	$(BUILD_TYPE)/$(TARGET_NAME).out	\
	$(BUILD_TYPE)/$(TARGET_NAME).bin	\
	$(BUILD_TYPE)/$(TARGET_NAME).hex

relink: $(BUILD_TYPE)
	$(RM) $(BUILD_TYPE)/$(TARGET_NAME).out
	$(RM) $(BUILD_TYPE)/$(TARGET_NAME).bin
	$(RM) $(BUILD_TYPE)/$(TARGET_NAME).hex
	make -C .

clean:
	$(RM) $(BUILD_TYPE)

info:
	@echo "BUILD_TYPE         = $(BUILD_TYPE)"
	@echo "NORDIC_DEVICE      = $(NORDIC_DEVICE) $(NORDIC_DEVICE_LOWER)"
	@echo "NORDIC_SDK_VERSION = $(NORDIC_SDK_VERSION)"
	@echo "BOARD_TYPE         = $(BOARD_TYPE)"
	@echo "CPU_TYPE           = $(CPU_TYPE)"
	@echo "LINKER_SCRIPT      = $(LINKER_SCRIPT)"
	@echo "NORDIC_DFU_SUPPORT = $(NORDIC_DFU_SUPPORT)"
	@echo "NORDIC_SDK_ROOT    = $(NORDIC_SDK_ROOT)"
	@echo "SOFT_DEVICE_ROOT   = $(SOFT_DEVICE_ROOT)"


include ../common/rules/arm_gcc_rules.mak
include ../common/rules/jlink_rules.mak

