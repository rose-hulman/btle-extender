###
# os_rules.make
# Determine operating the operatiing system and
# operating system specifics.
###

OS_NAME=$(shell uname)

ifeq ($(OS_NAME), Darwin)
	NUMBER_OF_PROCESSORS = $(shell sysctl -n hw.ncpu)
endif

ifeq ($(OS_NAME), Linux)
	NUMBER_OF_PROCESSORS := $(shell awk '/^processor/ {++n} END {print n}' /proc/cpuinfo)
endif

ifeq (,$(NUMBER_OF_PROCESSORS))
	MAKE_JOBS :=
else
	MAKE_JOBS := -j $(NUMBER_OF_PROCESSORS)
endif

