# This is the path to where you have your copy of the HIDAPI source.
HIDAPI_DIR=../hidapi.git

# This should point to one of the hid.o files under the HID API dir using a
# relative path. On Linux, this is either linux/hid.o or linux/hid-libusb.o
# depending on which variant you want to use. By default, this tries to
# autodetect which version you've built, by using a wildcard.
ifeq ($(shell uname), Darwin)
HIDAPI_OBJ=mac/hid*.o
else
HIDAPI_OBJ=linux/hid*.o
endif

# This ensures we have exactly one object file for HIDAPI.
HIDAPI_LIB=$(strip $(lastword $(wildcard $(HIDAPI_DIR)/$(HIDAPI_OBJ))))
ifeq ($(HIDAPI_LIB),)
$(error You must build HIDAPI before building this)
endif

CC=gcc
CFLAGS+=-I$(HIDAPI_DIR)/hidapi -Wall -g -fpic
LIBS=-lm

ifeq ($(HIDAPI_LIB),$(HIDAPI_DIR)/linux/hid-libusb.o)
CFLAGS+=`pkg-config libusb-1.0 --cflags`
LIBS+=`pkg-config libusb-1.0 --libs`
else
LIBS+=`pkg-config libudev --libs`
endif

LIB_SOURCES=$(shell find libtempered -name \*.c)
LIB_OBJECTS=$(patsubst libtempered/%.c,build/%.o,$(LIB_SOURCES))

PROGRAMS=$(patsubst %.c,%,$(wildcard utils/*.c))

all: $(PROGRAMS)

lib: libtempered.a libtempered.so

$(HIDAPI_LIB):
	$(error You must build HIDAPI before building this)

build/%.o: libtempered/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

libtempered.a: $(LIB_OBJECTS)
	[ -e $@ ] && rm -f $@ || true
	ar rc $@ $^
	ranlib $@

libtempered.so: $(LIB_OBJECTS)
	$(CC) $(CFLAGS) -shared -Wl,-soname,$@.0 -o $@ $^

$(PROGRAMS): %: %.c libtempered.a $(HIDAPI_LIB)
	$(CC) -Ilibtempered $(CFLAGS) $^ $(LIBS) -o $@

clean:
	rm -rf build libtempered.a libtempered.so $(PROGRAMS)

.PHONY: all clean lib
