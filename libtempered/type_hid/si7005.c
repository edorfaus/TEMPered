#include <stdbool.h>
#include <string.h>

#include "type-info.h"
#include "../tempered-internal.h"

bool tempered_type_hid_get_temperature_si7005(
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
	
	// Convert from two separate data bytes to a single integer.
	// The result should be an unsigned int between 0x0000 and 0xFFFF.
	int low_byte_offset = sensor->temperature_low_byte_offset;
	int high_byte_offset = sensor->temperature_high_byte_offset;
	int temp = ( group_data->data[low_byte_offset] & 0xFF )
		+ ( ( group_data->data[high_byte_offset] & 0xFF ) << 8 )
	;
	
	// According to the Silicon Labs Si7005 datasheet, there's 32 codes per ℃
	// with 0x0000 = -50℃.
	*tempC = ((float)temp) / 32 - 50;
	
	return true;
}

bool tempered_type_hid_get_humidity_si7005(
	tempered_device *device, struct tempered_type_hid_sensor *sensor,
	struct tempered_type_hid_query_result *group_data, float *rel_hum
) {
	float tempC;
	if (
		!tempered_type_hid_get_temperature_si7005(
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
	
	int low_byte_offset = sensor->humidity_low_byte_offset;
	int high_byte_offset = sensor->humidity_high_byte_offset;
	int rh = ( group_data->data[low_byte_offset] & 0xFF )
		+ ( ( group_data->data[high_byte_offset] & 0xFF ) << 8 )
	;
	
	// These formulas and values are based on the Silicon Labs Si7005 datasheet
	
	// There's 16 codes per %RH, with 0x0000 = -24%RH
	float relhum = ((float)rh) / 16 - 24;
	
	// Linearization
	relhum -= -0.00393 * relhum * relhum + 0.4008 * relhum - 4.7844;
	
	// Temperature compensation
	relhum += ( tempC - 30 ) * ( 0.00237 * relhum + 0.1973 );
	
	*rel_hum = relhum;
	
	return true;
}
