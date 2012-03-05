#ifndef TEMPER_TYPE_HID_1130_660C_H
#define TEMPER_TYPE_HID_1130_660C_H

#include "../tempered.h"
#include "common.h"

/** Type-specific data for 1130:660c HID devices. */
struct temper_type_hid_1130_660c_data {
	unsigned char id;
	char *name;
	struct temper_type_hid_data data;
};

/** Method for opening 1130:660c HID devices. */
bool temper_type_hid_1130_660c_open( tempered_device* device );

#endif
