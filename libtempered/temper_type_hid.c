#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "temper_type_hid.h"

#include "tempered.h"
#include "tempered-internal.h"

/** The struct that is stored in device->data for this type of device. */
struct temper_type_hid_device_data
{
	/** Handle for the HID device. */
	hid_device *hid_dev;
	
	/** Length of data that has been read from the device. */
	int data_length;
	
	/** The data that has been read from the device. */
	unsigned char data[64];
};

bool temper_type_hid_open( tempered_device* device )
{
	struct temper_type_hid_device_data *device_data = malloc(
		sizeof( struct temper_type_hid_device_data )
	);
	if ( device_data == NULL )
	{
		tempered_set_error(
			device, strdup( "Failed to allocate memory for the device data." )
		);
		return false;
	}
	device_data->hid_dev = hid_open_path( device->path );
	if ( device_data->hid_dev == NULL )
	{
		free( device_data );
		tempered_set_error( device, strdup( "Failed to open HID device." ) );
		return false;
	}
	device_data->data_length = 0;
	device->data = device_data;
	return true;
}

void temper_type_hid_close( tempered_device* device )
{
	struct temper_type_hid_device_data *device_data =
		(struct temper_type_hid_device_data *) device->data;
	hid_close( device_data->hid_dev );
	free( device_data );
}

bool temper_type_hid_read_sensors( tempered_device* device )
{
	struct temper_type_hid_data *info =
		(struct temper_type_hid_data*) device->type->data;
	
	struct temper_type_hid_device_data *device_data =
		(struct temper_type_hid_device_data *) device->data;
	
	hid_device *hid_dev = device_data->hid_dev;
	
	int size = hid_write( hid_dev, info->report_data, info->report_length );
	if ( size <= 0 )
	{
		size = snprintf(
			NULL, 0, "HID write failed: %ls",
			hid_error( hid_dev )
		);
		// TODO: check that size >= 0
		size++;
		char *error = malloc( size );
		size = snprintf(
			error, size, "HID write failed: %ls",
			hid_error( hid_dev )
		);
		tempered_set_error( device, error );
		device_data->data_length = 0;
		return false;
	}
	size = hid_read_timeout(
		hid_dev, device_data->data, sizeof( device_data->data ), 1000
	);
	if ( size < 0 )
	{
		size = snprintf(
			NULL, 0, "Read of data from the sensor failed: %ls",
			hid_error( hid_dev )
		);
		// TODO: check that size >= 0
		size++;
		char *error = malloc( size );
		size = snprintf(
			error, size, "Read of data from the sensor failed: %ls",
			hid_error( hid_dev )
		);
		tempered_set_error( device, error );
		device_data->data_length = 0;
		return false;
	}
	device_data->data_length = size;
	if ( size == 0 )
	{
		tempered_set_error(
			device, strdup( "No data was read from the sensor (timeout)." )
		);
		return false;
	}
	return true;
}

bool temper_type_hid_get_temperature(
	tempered_device* device, int sensor, float* tempC
) {
	struct temper_type_hid_data *info =
		(struct temper_type_hid_data*) device->type->data;
	
	struct temper_type_hid_device_data *device_data =
		(struct temper_type_hid_device_data *) device->data;
	
	if (
		device_data->data_length <= info->temperature_high_byte_offset ||
		device_data->data_length <= info->temperature_low_byte_offset
	)
	{
		tempered_set_error(
			device, strdup( "Not enough data was read from the sensor." )
		);
		return false;
	}
	
	// This calculation is based on the FM75 datasheet, and converts
	// from two separate data bytes to a single integer, which is
	// needed for all currently supported temperature sensors.
	int low_byte_offset = info->temperature_low_byte_offset;
	int high_byte_offset = info->temperature_high_byte_offset;
	int temp = ( device_data->data[low_byte_offset] & 0xFF )
		+ ( (signed char)device_data->data[high_byte_offset] << 8 )
	;
	
	// We currently only know about two types of underlying sensors, where one
	// supports humidity and the other doesn't, so for simplicity we're using
	// the has-humidity flag to decide which sensor's formula to use.
	if ( info->has_humidity )
	{
		// This temperature formula is based on the Sensirion SHT1x datasheet,
		// and uses the high-resolution numbers; low-resolution is probably
		// not really relevant for our uses.
		// We're here using d1 for VDD = 3.5V, as that matches best.
		*tempC = -39.7 + 0.01 * temp;
	}
	else
	{
		// This temperature formula is taken from the FM75 datasheet.
		// This is the same as dividing by 256; basically moving the
		// decimal point into place.
		*tempC = temp * 125.0 / 32000.0;
	}
	
	return true;
}

bool temper_type_hid_get_humidity(
	tempered_device* device, int sensor, float* rel_hum
) {
	struct temper_type_hid_data *info =
		(struct temper_type_hid_data*) device->type->data;
	
	if ( !info->has_humidity )
	{
		tempered_set_error(
			device, strdup( "This device does not have a humidity sensor." )
		);
		return false;
	}
	
	float tempC;
	if ( !temper_type_hid_get_temperature( device, sensor, &tempC ) )
	{
		return false;
	}
	
	struct temper_type_hid_device_data *device_data =
		(struct temper_type_hid_device_data *) device->data;
	
	if (
		device_data->data_length <= info->humidity_high_byte_offset ||
		device_data->data_length <= info->humidity_low_byte_offset
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
	int rh = ( device_data->data[info->humidity_low_byte_offset] & 0xFF )
		+ ( ( device_data->data[info->humidity_high_byte_offset] & 0xFF ) << 8 )
	;
	float relhum = -2.0468 + 0.0367 * rh - 1.5955e-6 * rh * rh;
	relhum = ( tempC - 25 ) * ( 0.01 + 0.00008 * rh ) + relhum;
	// Clamp the numbers to a sensible range, as per the datasheet.
	if ( relhum <= 0 ) relhum = 0;
	if ( relhum > 99 ) relhum = 100;
	
	*rel_hum = relhum;
	
	return true;
}
