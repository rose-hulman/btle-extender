###
# File: jlink_rules.mak
# Makefile rules for executing JLinkExe (aka JLink Commander) commands.
#
# Parameters:
#	BUILD_TYPE	Debug			Default
#			Release
#	TARGET_NAME				The binary file (elf) name,
#						determines the name of the .hex and .out files.
#	NORDIC_DEVICE				The NRF51 or NRF52 device type
#	NORDIC_SDK_ROOT
#	SOFT_DEVICE	S312			The nRF52 Softdevice
#			S130			The nRF51 Softdevice
#	SOFT_DEVICE_LOWER			Lower case version of softdevice
###

SOFT_DEVICE_ROOT		?= $(NORDIC_SDK_ROOT)/components/softdevice/$(SOFT_DEVICE_LOWER)

###
# Note: only set up for SDK 11 at the moment.
# TODO: Make this for SDK 10.
###
ifeq ($(NORDIC_SDK_VERSION), 11)
ifeq ($(NORDIC_DEVICE), NRF51)
SOFT_DEVICE_HEX_FILE		:= $(SOFT_DEVICE_ROOT)/hex/s130_nrf51_2.0.0-7.alpha_softdevice.hex
endif

ifeq ($(NORDIC_DEVICE), NRF52)
SOFT_DEVICE_HEX_FILE		:= $(SOFT_DEVICE_ROOT)/hex/s132_nrf52_2.0.0-7.alpha_softdevice.hex
endif
endif

###
# Nordic CONFIG, ERASEALLm ERASEUICR registers.
# See nRF51 Reference Manual Section 6 NVMC
# See nRF52832 Objective Product Specification, section 10 NVMC
###
NRF51_REG_CONFIG		:= 0x4001e504
NRF51_REG_CONFIG_REN		:= 0
NRF51_REG_CONFIG_WEN		:= 1
NRF51_REG_CONFIG_EEN		:= 2

NRF51_REG_ERASEALL		:= 0x4001e50c
NRF51_REG_ERASEALL_NOOP 	:= 1
NRF51_REG_ERASEALL_ERASE	:= 1

NRF51_REG_ERASEUICR		:= 0x4001e514
NRF51_REG_ERASEUICR_NOOP 	:= 1
NRF51_REG_ERASEUICR_ERASE	:= 1

# By default do not print out the toolchain details.
NO_ECHO ?= @

###
# Sudo is only required for Linux systems.
# (see README.md for configuration options)
###
ifneq (,$(findstring Linux,$(shell uname)))
	SUDO	:= sudo
else
	SUDO	:=
endif

###
# The build type: Debug or Release (case sensetive)
# This will be the artifact (.bin, .hex, .map, .o, etc.) output directory
###
BUILD_TYPE	?= Debug

###
# The installed location of the Segger JLink tools
# OSX installation directory differs from 'nix installs.
###
ifneq (,$(findstring Darwin,$(shell uname)))
	SEGGER_JLINK_ROOT := /Applications/SEGGER/JLink
else
	SEGGER_JLINK_ROOT := C:/Program Files (x86)/SEGGER/JLink_V502
endif

###
# Segger JLink commands
###
JLINK_EXE		:= "$(SEGGER_JLINK_ROOT)/JLink.exe"
JLINK_GDB_SERVER	:= "$(SEGGER_JLINK_ROOT)/JLinkGDBServer.exe"

###
# Segger Toolchain commands
###
JLINK_OPTS	+= -device $(NORDIC_DEVICE)
JLINK_OPTS	+= -if swd
JLINK_OPTS	+= -speed 1000

###
# Segger GDB Server
# -vd			Verify download data.
# -ir			Initialize CPU registers on start.
# -localhostonly	1: Allow locahost connections only.
#			0: Allow remote   connections.
# -strict		Exit on invalid parameters in start
###
JLINK_GDB_OPTS	+= -endian little
JLINK_GDB_OPTS	+= -port 2331
JLINK_GDB_OPTS	+= -swoport 2332
JLINK_GDB_OPTS	+= -telnetport 2333
JLINK_GDB_OPTS	+= -vd
JLINK_GDB_OPTS	+= -noir
JLINK_GDB_OPTS	+= -localhostonly 0
JLINK_GDB_OPTS	+= -strict
JLINK_GDB_OPTS	+= -timeout 0

.PHONY: run-debug run flash-all flash flash-softdevice flash-erase flash-erase-all flash-app-valid flash-app-invalid jlink-help jlink-info

###
# run-debug does not have any dependencies since you may
# want to attach to the debugger without resetting the target.
###
run-debug:
	$(SUDO) $(JLINK_GDB_SERVER) $(JLINK_OPTS) $(JLINK_GDB_OPTS)

run: $(BUILD_TYPE)/run_target.jlink
	$(SUDO) $(JLINK_EXE) $(JLINK_OPTS) $^
	$(NO_ECHO) $(RM) $^

###
# JLink related flashing and debug init commands
###
flash-all: flash-softdevice flash

flash: $(BUILD_TYPE)/flash_target.jlink
	$(SUDO) $(JLINK_EXE) $(JLINK_OPTS) $^
	$(NO_ECHO) $(RM) $^

flash-softdevice: $(BUILD_TYPE)/flash_softdevice.jlink
	$(SUDO) $(JLINK_EXE) $(JLINK_OPTS) $^
	$(NO_ECHO) $(RM) $^

flash-erase: $(BUILD_TYPE)/flash_erase.jlink
	$(SUDO) $(JLINK_EXE) $(JLINK_OPTS) $^
	$(NO_ECHO) $(RM) $^

flash-erase-all: $(BUILD_TYPE)/flash_erase_all.jlink
	$(SUDO) $(JLINK_EXE) $(JLINK_OPTS) $^
	$(NO_ECHO) $(RM) $^

flash-app-valid: $(BUILD_TYPE)/flash_app_valid.jlink
	$(SUDO) $(JLINK_EXE) $(JLINK_OPTS) $^
	$(NO_ECHO) $(RM) $^

flash-app-invalid: $(BUILD_TYPE)/flash_app_invalid.jlink
	$(SUDO) $(JLINK_EXE) $(JLINK_OPTS) $^
	$(NO_ECHO) $(RM) $^

$(BUILD_TYPE)/flash_target.jlink: $(BUILD_TYPE)/$(TARGET_NAME).hex
	$(NO_ECHO) printf "Fashing $<\n"
	$(NO_ECHO) printf "speed 1000\n"    > $@
	$(NO_ECHO) printf "loadfile $<\n"  >> $@
	$(NO_ECHO) printf "r\n"            >> $@
	$(NO_ECHO) printf "exit\n"         >> $@

$(BUILD_TYPE)/run_target.jlink: $(BUILD_TYPE)/$(TARGET_NAME).hex
	$(NO_ECHO) printf "Fashing and running $<\n"
	$(NO_ECHO) printf "speed 1000\n"    > $@
	$(NO_ECHO) printf "loadfile $<\n"  >> $@
	$(NO_ECHO) printf "r\n"            >> $@
	$(NO_ECHO) printf "g\n"            >> $@

$(BUILD_TYPE)/flash_softdevice.jlink: | $(BUILD_TYPE)
	$(NO_ECHO) printf "Flashing $(SOFT_DEVICE_HEX_FILE)\n"
	$(NO_ECHO) printf "speed 1000\n"    > $@
	$(NO_ECHO) printf "loadfile $(SOFT_DEVICE_HEX_FILE)\n" >> $@
	$(NO_ECHO) printf "exit\n"         >> $@

$(BUILD_TYPE)/flash_erase.jlink:
	$(NO_ECHO) printf "erase\n"         > $@
	$(NO_ECHO) printf "r\n"            >> $@
	$(NO_ECHO) printf "exit\n"         >> $@

# Erase EVERYTHING.
$(BUILD_TYPE)/flash_erase_all.jlink:
	$(NO_ECHO) printf "Erasing all flash\n"
	$(NO_ECHO) printf "w4 $(NRF51_REG_CONFIG)    $(NRF51_REG_CONFIG_EEN)\n"       > $@
	$(NO_ECHO) printf "w4 $(NRF51_REG_ERASEALL)  $(NRF51_REG_ERASEALL_ERASE)\n"  >> $@
	$(NO_ECHO) printf "exit\n"                                                   >> $@

# Indicate to the bootloader that the application is valid
$(BUILD_TYPE)/flash_app_valid.jlink:
	$(NO_ECHO) printf "h\n"                     > $@
	$(NO_ECHO) printf "w4 0x0003fc00 0x0001\n" >> $@
	$(NO_ECHO) printf "w4 0x0003fc04 0x0000\n" >> $@
	$(NO_ECHO) printf "w4 0x0003fc08 0x00fe\n" >> $@
	$(NO_ECHO) printf "w4 0x0003fc0c 0x0000\n" >> $@
	$(NO_ECHO) printf "w4 0x0003fc10 0x0000\n" >> $@
	$(NO_ECHO) printf "w4 0x0003fc14 0x0000\n" >> $@
	$(NO_ECHO) printf "w4 0x0003fc18 0x0000\n" >> $@
	$(NO_ECHO) printf "w4 0x0003fc1c 0x0000\n" >> $@
	$(NO_ECHO) printf "r\n"                    >> $@
	$(NO_ECHO) printf "exit\n"                 >> $@

# Indicate to the bootloader that the application is valid
$(BUILD_TYPE)/flash_app_invalid.jlink:
	$(NO_ECHO) printf "h\n"                     > $@
	$(NO_ECHO) printf "w4 0x0003fc00 0x0000\n" >> $@
	$(NO_ECHO) printf "w4 0x0003fc04 0x0000\n" >> $@
	$(NO_ECHO) printf "w4 0x0003fc08 0x00fe\n" >> $@
	$(NO_ECHO) printf "w4 0x0003fc0c 0x0000\n" >> $@
	$(NO_ECHO) printf "w4 0x0003fc10 0x0000\n" >> $@
	$(NO_ECHO) printf "w4 0x0003fc14 0x0000\n" >> $@
	$(NO_ECHO) printf "w4 0x0003fc18 0x0000\n" >> $@
	$(NO_ECHO) printf "w4 0x0003fc1c 0x0000\n" >> $@
	$(NO_ECHO) printf "r\n"                    >> $@
	$(NO_ECHO) printf "exit\n"                 >> $@

jlink-help:
	@echo "targets:"
	@echo "flash-all       : flash the build target and softdevice"
	@echo "flash           : flash just the build target (FM beacon app or bootloader)"
	@echo "flash-softdevice: Flash the softdevice run-time"
	@echo "flash-erase:    : Erase flash"
	@echo "flash-erase-all : Erase all flash using special Nordic registers"
	@echo "flash-app-valid : Mark the Nordic application flash bits are valid"
	@echo "run-debug       : Start the gdb server - resets the device"

jlink-info:
	@echo "SOFT_DEVICE_HEX_FILE = $(SOFT_DEVICE_HEX_FILE)"
