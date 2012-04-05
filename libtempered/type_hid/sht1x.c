#include <stdbool.h>
#include <string.h>

#include "type-info.h"
#include "../tempered-internal.h"

bool tempered_type_hid_get_temperature_sht1x(
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
	
	// This temperature formula is based on the Sensirion SHT1x datasheet,
	// and uses the high-resolution numbers; low-resolution is probably
	// not really relevant for our uses.
	// We're here using d1 for VDD = 3.5V, as that matches best for the devices
	// we currently support.
	*tempC = -39.7 + 0.01 * temp;
	
	return true;
}

bool tempered_type_hid_get_humidity_sht1x(
	tempered_device *device, struct tempered_type_hid_sensor *sensor,
	struct tempered_type_hid_query_result *group_data, float *rel_hum
) {
	float tempC;
	if (
		!tempered_type_hid_get_temperature_sht1x(
			device, sensor, group_data, &tempC
		)
	) {
		return false;
	}
	
	if (
		group_data->length <= sensor->humidity_high_byte_offset ||
		group_data->length <= sensor->humidity_low_byte_offset
	)
	{
		tempered_set_error(
			device, strdup( "Not enough data was read from the sensor." )
		);
		return false;
	}
	
	// These formulas are based on the Sensirion SHT1x datasheet,
	// and uses the high-resolution numbers; low-resolution is
	// probably not really relevant for our uses.
	
	// Relative humidity.
	int low_byte_offset = sensor->humidity_low_byte_offset;
	int high_byte_offset = sensor->humidity_high_byte_offset;
	int rh = ( group_data->data[low_byte_offset] & 0xFF )
		+ ( ( group_data->data[high_byte_offset] & 0xFF ) << 8 )
	;
	float relhum = -2.0468 + 0.0367 * rh - 1.5955e-6 * rh * rh;
	relhum = ( tempC - 25 ) * ( 0.01 + 0.00008 * rh ) + relhum;
	// Clamp the numbers to a sensible range, as per the datasheet.
	if ( relhum <= 0 ) relhum = 0;
	if ( relhum > 99 ) relhum = 100;
	
	*rel_hum = relhum;
	
	return true;
}
