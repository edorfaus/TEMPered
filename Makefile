# This is the path to where you have your copy of the HIDAPI source.
HIDAPI_DIR=../hidapi.git

# This should point to one of the hid.o files under the HID API dir using a
# relative path. On Linux, this is either linux/hid.o or linux/hid-libusb.o
# depending on which variant you want to use.
HIDAPI_OBJ=linux/hid.o

CC=gcc
CFLAGS+=-I$(HIDAPI_DIR)/hidapi -Wall -g `pkg-config libusb-1.0 --cflags`
HIDAPI_LIB=$(HIDAPI_DIR)/$(HIDAPI_OBJ)
LIBS=`pkg-config libusb-1.0 libudev --libs` -lm

LIB_OBJECTS=$(patsubst libtempered/%.c,build/%.o,$(wildcard libtempered/*.c))

PROGRAMS=$(patsubst %.c,%,$(wildcard utils/*.c))

all: $(PROGRAMS)

$(HIDAPI_LIB):
	$(error You must build HIDAPI before building this)

build/.directory:
	mkdir -p build
	@touch build/.directory

build/%.o: libtempered/%.c build/.directory
	$(CC) $(CFLAGS) -c $< -o $@

libtempered.a: $(LIB_OBJECTS)
	ar rc $@ $^
	ranlib $@

$(PROGRAMS): %: %.c libtempered.a $(HIDAPI_LIB)
	$(CC) -Ilibtempered $(CFLAGS) $^ $(LIBS) -o $@

clean:
	rm -rf build libtempered.a $(PROGRAMS)

.PHONY: all clean
