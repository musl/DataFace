########################################################################
# Setup
########################################################################
#
# Yes, the pebble tool is fantastic, but I like to automate things.
# This lets me run `make` to run the app on my wrist and watch the
# logs directly from the device. Nifty. YMMV.
#

# Declare tasks that don't result in something that make can check for
# freshness.
.PHONY: all check_env clean install logs

# A default list of things to do when no arguments are given.
all: check_env clean logs

########################################################################
# Tasks
########################################################################
#
# Feel free to run these individually as appropriate.
# For example:
#     make clean
#     make build
#

# Make sure that the tool know where your phone is.
#
check_env:
ifndef PEBBLE_PHONE
	$(error PEBBLE_PHONE is not defined)
endif

clean:
	pebble clean

build:
	pebble build

install: check_env build
	pebble install

logs: check_env install
	pebble logs

