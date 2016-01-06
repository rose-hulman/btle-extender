###
# File: arm_gcc_rules.mak
#
# Makefile definitions and rules for the ARM GCC toolchain.
#
# Include a set of C/Assembler file definitions that in the end will define
# the following:
#	TARGET_NAME			The build target executable name.
#	I_PATHS				C-Language Include paths (without -I prefix)
#	C_SOURCE_FILES			C-Language Source path/files
#	S_SOURCE_FILES			ARM Assembler Source path/files
#	LD_PATH				Linker path file include specification
#	BOARD_TYPE
###

# By default do not print out the toolchain details.
NO_ECHO ?= @

###
# The build type: Debug or Release (case sensetive)
# This will be the artifact (.bin, .hex, .map, .o, etc.) output directory
###
BUILD_TYPE	?= Debug

###
# Set up the location of the gcc ARM cross-compiler.
# This will change with the version of compiler being used.
###
GNU_PREFIX	:= arm-none-eabi
GNU_GCC_ROOT	:= /usr/local/gcc-$(GNU_PREFIX)
GNU_VERSION     := 4.9.3

###
# GCC Toolchain commands
###
CC		:= "$(GNU_GCC_ROOT)/bin/$(GNU_PREFIX)-gcc"
AS		:= "$(GNU_GCC_ROOT)/bin/$(GNU_PREFIX)-as"
AR		:= "$(GNU_GCC_ROOT)/bin/$(GNU_PREFIX)-ar" -r
LD		:= "$(GNU_GCC_ROOT)/bin/$(GNU_PREFIX)-ld"
NM		:= "$(GNU_GCC_ROOT)/bin/$(GNU_PREFIX)-nm"
OBJDUMP		:= "$(GNU_GCC_ROOT)/bin/$(GNU_PREFIX)-objdump"
OBJCOPY		:= "$(GNU_GCC_ROOT)/bin/$(GNU_PREFIX)-objcopy"
SIZE		:= "$(GNU_GCC_ROOT)/bin/$(GNU_PREFIX)-size"

###
# Other shell commands used
###
MKDIR		:= mkdir -v
RM		:= rm -rf

###
# Function for removing duplicates in a list - taken from Nordic's makefile exmaples.
###
remduplicates = $(strip $(if $1,$(firstword $1) $(call remduplicates,$(filter-out $(firstword $1),$1))))

# List of all C and assembler language file directories.
# As noted by the _V_ use these in the .vpath directives below.
# This tells the compiler where it can find source files.
C_V_PATHS = $(call remduplicates, $(dir $(C_SOURCE_FILES)))
S_V_PATHS = $(call remduplicates, $(dir $(S_SOURCE_FILES)))

vpath %.c $(C_V_PATHS)
vpath %.s $(S_V_PATHS)

# The complete list of C Language directories: source files + header files.
# Using remduplicates insures that the directory only gets used once.
# Not that it is necessary but it does clean things up.
C_I_PATHS += $(call remduplicates, $(I_PATHS) $(C_V_PATHS))

# The include path statement used by the compiler.
INCLUDE_PATHS := $(addprefix -I, $(C_I_PATHS))

# C language optimization flags
CFLAGS += -DBUILD_TYPE=$(BUILD_TYPE)
ifeq ("$(BUILD_TYPE)", "Debug")
	OPT_FLAGS = -Os
else
	OPT_FLAGS = -Os
endif

###
# Generic GCC flags
###
CFLAGS += --std=gnu99
CFLAGS += -Wall -Werror -g
CFLAGS += $(OPT_FLAGS)

###
# Configure the linker
###
LINKER_PATHS := $(addprefix -L, $(LD_PATHS))

# keep every function in separate section. This will allow linker to dump unused functions
LDFLAGS += -Xlinker -Map=$(BUILD_TYPE)/$(TARGET_NAME).map
LDFLAGS += $(LINKER_PATHS) -T $(LINKER_SCRIPT)
# let linker dump unused sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs -lc -lnosys

###
# Assembler flags
###
SFLAGS += -x assembler-with-cpp
SFLAGS += -D__HEAP_SIZE=0
#SFLAGS += -D__STACK_SIZE=2048

## Create build directories
$(BUILD_TYPE):
	$(NO_ECHO)$(MKDIR) $@

###
# Exclude paths from the source file names.
# This will result in a list of source files to compile.
###
C_SOURCE_FILE_NAMES = $(notdir $(C_SOURCE_FILES))
S_SOURCE_FILE_NAMES = $(notdir $(S_SOURCE_FILES))

###
# Convert source file names to their resulting ojbect file output.
# This will be the list of obejcts that need to be compiled and linked.
###
C_OBJECTS = $(addprefix $(BUILD_TYPE)/, $(C_SOURCE_FILE_NAMES:.c=.o) )
S_OBJECTS = $(addprefix $(BUILD_TYPE)/, $(S_SOURCE_FILE_NAMES:.s=.o) )

C_DEPENDS = $(C_OBJECTS:.o=.dep)

###
# The complete list of object files with build artifact directory prefix
###
OBJECTS = $(C_OBJECTS) $(S_OBJECTS)
$(OBJECTS): | $(BUILD_TYPE)

# Compile C SRC files
$(BUILD_TYPE)/%.o: %.c
	@echo Compiling file: $<
	$(NO_ECHO)$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c -o $@ $<
	$(NO_ECHO)$(CC) $(CFLAGS) $(INCLUDE_PATHS) -MM -MT $@ -MF $(@:.o=.dep) $<

# Compile Assembly files
$(BUILD_TYPE)/%.o: %.s
	@echo Assembling file: $<
	$(NO_ECHO)$(CC) $(SFLAGS) $(INCLUDE_PATHS) -c -o $@ $<

# Link
$(BUILD_TYPE)/$(TARGET_NAME).out: $(OBJECTS)
	@echo Linking target: $@
	$(NO_ECHO)$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@
	$(NO_ECHO)$(SIZE) $@

## Create binary .bin file from the .out file
$(BUILD_TYPE)/%.bin: $(BUILD_TYPE)/%.out
	@echo Creating: $@
	$(NO_ECHO)$(OBJCOPY) -O binary $< $@

## Create binary .hex file from the .out file
$(BUILD_TYPE)/%.hex: $(BUILD_TYPE)/%.out
	@echo Creating: $@
	$(NO_ECHO)$(OBJCOPY) -O ihex $< $@

.PHONY:  echosize arm_gcc_info

echosize: $(BUILD_TYPE)/$(TARGET_NAME).out
	-@echo ""
	$(NO_ECHO)$(SIZE) $<
	-@echo ""

arm_gcc_info:
	@echo
	@echo "INCLUDE_PATHS = "
	@echo $(INCLUDE_PATHS)	| tr ' ' '\n'
	@echo
	@echo "C_V_CPATHS = "
	@echo $(C_V_PATHS)	| tr ' ' '\n'
	@echo
	@echo "I_PATHS = "
	@echo $(I_PATHS)	| tr ' ' '\n'
	@echo
	@echo "C_I_PATHS = "
	@echo $(C_I_PATHS)	| tr ' ' '\n'
	@echo
#	@echo "C_DEPENDS = "
#	@echo $(C_DEPENDS)	| tr ' ' '\n'
#	@echo
	@echo "C_SOURCE_FILES = "
	@echo $(C_SOURCE_FILES)	| tr ' ' '\n'
	@echo
	@echo "LDFLAGS = "
	@echo $(LDFLAGS)	| tr ' ' '\n'
	@echo
	@echo "LINKER_SCRIPT = $(LINKER_SCRIPT)"
	@echo

-include $(C_DEPENDS)

