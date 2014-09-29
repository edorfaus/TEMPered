#ifndef TEMPERED__TYPE_HID__SI7021_H
#define TEMPERED__TYPE_HID__SI7021_H

#include <stdbool.h>

#include "type-info.h"

// These methods are for the Silicon Labs Si7021 chip.

bool tempered_type_hid_get_temperature_si7021(
	tempered_device *device, struct tempered_type_hid_sensor *sensor,
	struct tempered_type_hid_query_result *group_data, float *tempC
);

bool tempered_type_hid_get_humidity_si7021(
	tempered_device *device, struct tempered_type_hid_sensor *sensor,
	struct tempered_type_hid_query_result *group_data, float *rel_hum
);

#endif
