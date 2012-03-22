#ifndef TEMPERED__TYPE_HID__NTC_H
#define TEMPERED__TYPE_HID__NTC_H

#include <stdbool.h>

#include "type-info.h"

bool tempered_type_hid_read_sensor_group_ntc(
	tempered_device* device, struct tempered_type_hid_sensor_group* group,
	struct tempered_type_hid_query_result* group_data
);

bool tempered_type_hid_get_temperature_ntc(
	tempered_device *device, struct tempered_type_hid_sensor *sensor,
	struct tempered_type_hid_query_result *group_data, float *tempC
);

#endif
