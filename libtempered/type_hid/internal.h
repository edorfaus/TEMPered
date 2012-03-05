#ifndef TEMPER_TYPE_HID_INTERNAL_H
#define TEMPER_TYPE_HID_INTERNAL_H

#include <hidapi.h>

#include "common.h"

/** The struct that is stored in device->data for this type of device. */
struct temper_type_hid_device_data
{
	/** Handle for the HID device. */
	hid_device *hid_dev;
	
	/** Length of data that has been read from the device. */
	int data_length;
	
	/** The data that has been read from the device. */
	unsigned char data[64];
	
	/** The HID device info for this device. */
	struct temper_type_hid_data *dev_info;
};

#endif
