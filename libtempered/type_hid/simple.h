#ifndef TEMPERED__TYPE_HID__SIMPLE_H
#define TEMPERED__TYPE_HID__SIMPLE_H

#include <stdbool.h>

#include "type-info.h"

bool tempered_type_hid_get_temperature_simple(
	tempered_device *device, struct tempered_type_hid_sensor *sensor,
	struct tempered_type_hid_query_result *group_data, float *tempC
);

#endif
