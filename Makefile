# This is the path to where you have your copy of the HIDAPI source.
HIDAPI_DIR=../hidapi.git

# This should point to one of the hid.o files under the HID API dir using a
# relative path. On Linux, this is either linux/hid.o or linux/hid-libusb.o
# depending on which variant you want to use.
HIDAPI_OBJ=linux/hid.o

# This is the list of programs that can/will be built.
PROGRAMS=temperhid enumerate

all: $(PROGRAMS)

CC=gcc
OBJS=
CFLAGS+=-I$(HIDAPI_DIR)/hidapi -Wall -g `pkg-config libusb-1.0 --cflags`
HIDAPI_LIB=$(HIDAPI_DIR)/$(HIDAPI_OBJ)
LIBS=`pkg-config libusb-1.0 libudev --libs`


$(HIDAPI_LIB):
	$(error You must build HIDAPI before building this)

$(PROGRAMS): %: %.c $(OBJS) $(HIDAPI_LIB)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(PROGRAMS)

.PHONY: clean
