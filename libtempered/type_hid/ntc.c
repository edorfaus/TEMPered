#include <stdbool.h>
#include <string.h>

#include "type-info.h"
#include "../tempered-internal.h"

bool tempered_type_hid_read_sensor_group_ntc(
	tempered_device* device, struct tempered_type_hid_sensor_group* group,
	struct tempered_type_hid_query_result* group_data
) {
	// TODO: implement NTC reading and temperature retrieval
	tempered_set_error(
		device, strdup( "Reading NTC sensors has not been implemented yet." )
	);
	return false;
}

bool tempered_type_hid_get_temperature_ntc(
	tempered_device *device, struct tempered_type_hid_sensor *sensor,
	struct tempered_type_hid_query_result *group_data, float *tempC
) {
	if (
		group_data->length <= sensor->temperature_high_byte_offset ||
		group_data->length <= sensor->temperature_low_byte_offset
	) {
		tempered_set_error(
			device, strdup( "Not enough data was read from the sensor." )
		);
		return false;
	}
	// TODO: implement NTC reading and temperature retrieval
	tempered_set_error(
		device, strdup( "Reading NTC sensors has not been implemented yet." )
	);
	return false;
}
