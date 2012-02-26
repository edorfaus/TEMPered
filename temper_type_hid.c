#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "temper_type_hid.h"

#include "tempered.h"
#include "tempered-internal.h"

bool temper_type_hid_open( tempered_device* device )
{
	hid_device *hid_dev = hid_open_path( device->path );
	if ( hid_dev == NULL )
	{
		tempered_set_error( device, strdup( "Failed to open HID device." ) );
		return false;
	}
	device->data = hid_dev;
	return true;
}

void temper_type_hid_close( tempered_device* device )
{
	hid_device *hid_dev = (hid_device*) device->data;
	hid_close( hid_dev );
}

bool temper_type_hid_get_temperature( tempered_device* device, float* tempC )
{
	struct temper_type_hid_data *info =
		(struct temper_type_hid_data*) device->type->data;
	hid_device *dev = (hid_device*) device->data;
	unsigned char data[64];
	int size;
	
	size = hid_write( dev, info->report_data, info->report_length );
	if ( size <= 0 )
	{
		size = snprintf(
			NULL, 0, "HID write failed: %ls",
			hid_error( dev )
		);
		// TODO: check that size >= 0
		size++;
		char *error = malloc( size );
		size = snprintf(
			error, size, "HID write failed: %ls",
			hid_error( dev )
		);
		tempered_set_error( device, error );
		return false;
	}
	size = hid_read_timeout( dev, data, sizeof(data), 1000 );
	if ( size < 0 )
	{
		size = snprintf(
			NULL, 0, "Read of data from the sensor failed: %ls",
			hid_error( dev )
		);
		// TODO: check that size >= 0
		size++;
		char *error = malloc( size );
		size = snprintf(
			error, size, "Read of data from the sensor failed: %ls",
			hid_error( dev )
		);
		tempered_set_error( device, error );
		return false;
	}
	if ( size == 0 )
	{
		tempered_set_error(
			device, strdup( "No data was read from the sensor (timeout)." )
		);
		return false;
	}
	if (
		size <= info->temperature_high_byte_offset ||
		size <= info->temperature_low_byte_offset
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
	int temp = ( data[info->temperature_low_byte_offset] & 0xFF )
		+ ( (signed char)data[info->temperature_high_byte_offset] << 8 )
	;
	
	// This is the same as dividing by 256; basically moving the
	// decimal point into place.
	// This formula is from the FM75 datasheet.
	*tempC = temp * 125.0 / 32000.0;
	
	return true;
}
