###
# File: nrf5x_defs.mak
#
# nRF51 and nRF52 Makefile definitions based on part number and model variant.
#
# The nRF51822xx compatability Matrix:
# nRF51 Variants:
# 		Flash		RAM
# AA		256 kB		16 kB
# AB		128 kB		16 kB
# AC		256 kB		32 kB
#
# The nRF52832 Variants (only AA at present)
# 		Flash		RAM
# AA		512 kB		64 kB
#
# Symbols intended to be set to non-default settings by the Makefile:
#	NORDIC_DFU_SUPPORT		1	Enable the Nordic DFU service in the application.
#					0	Disable the Nordic DFU service in the application.
#
# Symbols defined by this make include file:
#	NORDIC_SDK_ROOT			The directory containing the Nordic SDK.
#
#	BOARD_TYPE	PCA10028	nRF51	Default, nRF51
#			PCA10031	nRF51	nRF51 dongle (N51422QFACAB)
#			PCA10036	nRF52
#			PCA10040	nRF52	nRF52 with NFC
#
#	BOARD_TYPE_LOWER		BOARD_TYPE as lower case.
#
#	NORDIC_DEVICE	NRF51		Default
#			NRF52
#
#	NORDIC_DEVICE_LOWER		NORDIC_DEVICE as lower case.
#
#	NORDIC_LOG_TYPE	UART		Use the UART for console log output
#			RTT		Use the RTT for console log output	Default
#			RAW_UART
#			--none--	Set to any other value to disable logging
#
#	SOFT_DEVICE_ROOT		The location of the apropriate softevice library to use.
#
#	SOFT_DEVICE	S130		The nRF51 Softdevice (determined by NORDIC_DEVICE setting)
#			S132		The nRF52 Softdevice
#
#	SOFT_DEVICE_LOWER		SOFT_DEVICE as lower case.
###

###
# The device to build for.
# NORDIC_LOWER will be the lower case version of 'NRF5x'.
###
NORDIC_DEVICE		?= NRF51
NORDIC_DEVICE_LOWER	:= $(shell echo $(NORDIC_DEVICE) | tr A-Z a-z)

###
# The Nordic SDK version.
# Version 10 or 11 works.
###
NORDIC_SDK_VERSION	?= 11

###
# Set to 1 in order to enable the DFU service in a Nordic application.
###
NORDIC_DFU_SUPPORT	?= 0

###
# Where the Nordic SDK is installed.
# This will change based on SDK version.
###
ifeq ($(NORDIC_SDK_VERSION), 11)
NORDIC_SDK_ROOT		?= ../../SDK11
#/nRF5_SDK_11.0.0-2.alpha_bc3f6a0
endif

ifeq ($(NORDIC_SDK_VERSION), 10)
NORDIC_SDK_ROOT		?= ../../SDK10
#/nRF51_SDK_10.0.0_dc26b5e
endif

###
# - Select default Nordic variants based on the NORDIC_DEVICE.
# - Select a softdevice based on NORDIC_DEVICE.
#	Note that for nRF51 the softdevice S110 will not be supported in
#	future releases. The S310 takes its place.
#	See https://devzone.nordicsemi.com/question/61934/future-support-of-s110/
###
ifeq ($(NORDIC_DEVICE), NRF52)
BOARD_TYPE		?= PCA10040
SOFT_DEVICE		?= S132
CPU_TYPE		:=cortex-m4

else
BOARD_TYPE		?= PCA10028
SOFT_DEVICE		?= S130
CPU_TYPE		:=cortex-m0
endif

BOARD_TYPE_LOWER	:= $(shell echo $(BOARD_TYPE)     | tr A-Z a-z)
SOFT_DEVICE_LOWER	:= $(shell echo $(SOFT_DEVICE)    | tr A-Z a-z)

###
# Determine where the console log output should be sent.
###
NORDIC_LOG_TYPE		?= RTT

NORDIC_FLAGS += -D$(NORDIC_DEVICE)
NORDIC_FLAGS += -DNORDIC_SDK_VERSION=$(NORDIC_SDK_VERSION)
NORDIC_FLAGS += -DSOFTDEVICE_PRESENT
NORDIC_FLAGS += -DBOARD_$(BOARD_TYPE)
NORDIC_FLAGS += -DNRF_LOG_USES_$(NORDIC_LOG_TYPE)=1
NORDIC_FLAGS += -DCONFIG_GPIO_AS_PINRESET
NORDIC_FLAGS += -D$(SOFT_DEVICE)
NORDIC_FLAGS += -DSWI_DISABLE0
NORDIC_FLAGS += -DBLE_STACK_SUPPORT_REQD

ifeq ($(NORDIC_DFU_SUPPORT), 1)
NORDIC_FLAGS += -DBLE_DFU_APP_SUPPORT
endif

###
# PAN are related to nRF52 Errata
# See http://infocenter.nordicsemi.com/pdf/nRF52832_Errata_v1.2.pdf
# Each of these symbols being defined can be traced to an Errata item
# in that document. In SDK 11 I found:
#	- They only relate to nRF52 devices
#	- Only 23, 29, 30, 46 have functional relevance
#	  (The others are unused in SDK11)
###
ifeq ($(NORDIC_DEVICE), NRF52)
NORDIC_FLAGS += -DNRF52_PAN_12
NORDIC_FLAGS += -DNRF52_PAN_15
NORDIC_FLAGS += -DNRF52_PAN_20
NORDIC_FLAGS += -DNRF52_PAN_30
NORDIC_FLAGS += -DNRF52_PAN_31
NORDIC_FLAGS += -DNRF52_PAN_36
NORDIC_FLAGS += -DNRF52_PAN_51
NORDIC_FLAGS += -DNRF52_PAN_53
NORDIC_FLAGS += -DNRF52_PAN_54
NORDIC_FLAGS += -DNRF52_PAN_55
NORDIC_FLAGS += -DNRF52_PAN_58
NORDIC_FLAGS += -DNRF52_PAN_62
NORDIC_FLAGS += -DNRF52_PAN_63
NORDIC_FLAGS += -DNRF52_PAN_64
endif

# nRF52 CPU specific compiler settings
ifeq ("$(NORDIC_DEVICE)", "NRF52")
FPU_FLAGS += -mfloat-abi=hard
FPU_FLAGS += -mfpu=fpv4-sp-d16
endif

#nRF51 CPU specific compiler settings
ifeq ("$(NORDIC_DEVICE)", "NRF51")
FPU_FLAGS += -mfloat-abi=soft
endif

###
# These are common to ARM related settings.
# https://gcc.gnu.org/onlinedocs/gcc-4.9.3/gcc/Option-Summary.html#Option-Summary
# They could be CPU/processor specific - debateable - but common for now.
# -mabi=aapcs
#	http://infocenter.arm.com/help/topic/com.arm.doc.ihi0042f/IHI0042F_aapcs.pdf
#	AAPCS Procedure Call Standard for the ARM Architecture
# -ffunction-sections
# -fdata-sections
#	Enables the linker to dead strip unused code and data.
#	See linker flag: -Wl,--gc-sections
# -fno-strict-aliasing
#	Do not assume that pointers must point to different data when optimizing.
# -fno-builtin
#	Do no perform GCC optimization on standard library calls.
###
CFLAGS += $(NORDIC_FLAGS)
CFLAGS += -mcpu=$(CPU_TYPE)
CFLAGS += -mthumb
CFLAGS += -mabi=aapcs
CFLAGS += --short-enums
CFLAGS += -ffunction-sections
CFLAGS += -fdata-sections
CFLAGS += -fno-strict-aliasing
CFLAGS += -fno-builtin
CFLAGS += $(FPU_FLAGS)

###
# Assembler Flags specific to Nordic.
###
SFLAGS += $(NORDIC_FLAGS)

###
# Linker Flags specific to Nordic.
# -Wl,--gc-sections
#	Dead strip unused code and data.
###
LDFLAGS += -mcpu=$(CPU_TYPE)
LDFLAGS += -mthumb
LDFLAGS += -mabi=aapcs
LDFLAGS += $(FPU_FLAGS)
LDFLAGS += -Wl,--gc-sections

