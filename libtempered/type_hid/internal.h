#ifndef TEMPERED__TYPE_HID__INTERNAL_H
#define TEMPERED__TYPE_HID__INTERNAL_H

#include <hidapi.h>

#include "common.h"
#include "type-info.h"

/** The struct that is stored in device->data for this type of device. */
struct tempered_type_hid_device_data
{
	/** Handle for the HID device. */
	hid_device *hid_dev;
	
	/** Array of groups of data that has been read from the device. */
	struct tempered_type_hid_query_result *group_data;
};

/** Perform a HID query on the given device. */
bool tempered_type_hid_query(
	tempered_device* device, struct tempered_type_hid_query* query,
	struct tempered_type_hid_query_result* result
);

#endif
