########################################################################
# Setup
########################################################################
#
# Yes, the pebble tool is fantastic, but I like to automate things.
# This lets me run `make` to run the app on my wrist and watch the
# logs directly from the device. Nifty. YMMV.
#

ifndef PEBBLE_EMULATOR
PEBBLE_EMULATOR := basalt
endif

ifndef PEBBLE_PHONE
ifndef PEBBLE_CLOUDPEBBLE
PEBBLE_FLAGS := --logs --emulator $(PEBBLE_EMULATOR)
endif
endif

########################################################################
# Tasks
########################################################################

# Declare tasks that don't result in something that make can check for
# freshness.
.PHONY: all build clean install logs

# A default list of things to do when no arguments are given.
all: clean logs

clean:
	pebble $@

build:
	pebble $@

install: build
	pebble $@ $(PEBBLE_FLAGS)

logs: install
	pebble $@ $(PEBBLE_FLAGS)

