#ifndef TEMPERED__TYPE_HID__SHT1X_H
#define TEMPERED__TYPE_HID__SHT1X_H

#include <stdbool.h>

#include "type-info.h"

bool tempered_type_hid_get_temperature_sht1x(
	tempered_device *device, struct tempered_type_hid_sensor *sensor,
	struct tempered_type_hid_query_result *group_data, float *tempC
);

bool tempered_type_hid_get_humidity_sht1x(
	tempered_device *device, struct tempered_type_hid_sensor *sensor,
	struct tempered_type_hid_query_result *group_data, float *rel_hum
);

#endif
