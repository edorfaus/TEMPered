#include <stdbool.h>
#include <string.h>

#include "type-info.h"
#include "../tempered-internal.h"

bool tempered_type_hid_get_temperature_fm75(
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
	
	// This calculation is based on the FM75 datasheet, and converts
	// from two separate data bytes to a single integer.
	int low_byte_offset = sensor->temperature_low_byte_offset;
	int high_byte_offset = sensor->temperature_high_byte_offset;
	int temp = ( group_data->data[low_byte_offset] & 0xFF )
		+ ( (signed char)group_data->data[high_byte_offset] << 8 )
	;
	
	// This temperature formula is taken from the FM75 datasheet.
	// This is the same as dividing by 256; basically moving the
	// decimal point into place.
	*tempC = temp * 125.0 / 32000.0;
	
	return true;
}
