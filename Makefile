
.PHONY: all build clean install logs

all: clean build install logs

clean:
	pebble clean

build:
	pebble build

install:
	pebble install

logs:
	pebble logs

