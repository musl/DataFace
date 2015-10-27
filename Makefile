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
all: logs

########################################################################
# Tasks
########################################################################

check_env:
ifndef PEBBLE_PHONE
	$(error The environment variable PEBBLE_PHONE is not defined.\
		Please set it to the hostname or IP address for your phone)
endif

clean:
	pebble clean

pbw:
	pebble build

install: check_env pbw
	pebble install

logs: check_env install
	pebble logs

