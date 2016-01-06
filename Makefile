# BTLE top level Makefile

include common/rules/os_rules.mak

###
# Build each Footmarks target binary.
###
all: btle_peripheral btle_central

btle_peripheral:
	make -C ./peripheral		$(MAKE_JOBS)

btle_central:
	make -C ./central		$(MAKE_JOBS)

relink:
	make -C ./peripheral		$@
	make -C ./central		$@

clean:
	make -C ./central		$@
	make -C ./peripheral		$@

info:
	@echo "BUILD_TYPE           = $(BUILD_TYPE)"
	@echo "MAKE_JOBS            = $(MAKE_JOBS)"

	make -C ./peripheral		$@
	make -C ./central		$@

.PHONY: all btle_peripheral btle_central relink clean info

