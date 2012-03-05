#include <stdlib.h>
#include <stdbool.h>

#include "1130_660c.h"
#include "internal.h"

#include "../tempered.h"
#include "../tempered-internal.h"

bool temper_type_hid_1130_660c_open( tempered_device* device )
{
	if ( !temper_type_hid_open( device ) )
	{
		return false;
	}
	/*
	Note: after temper_type_hid_open(), the value of device->data->dev_info
	is invalid - it points to something other than what is expected. So, we
	have to reset it here.
	
	To do that properly, we first have to figure out which kind of device
	we're dealing with - since this code is used for multiple similar device
	types that all use the same USB IDs.
	*/
	
	struct temper_type_hid_device_data *device_data =
		(struct temper_type_hid_device_data *) device->data;
	
	// Set the device info to what we need to get the type ID from the device.
	device_data->dev_info = &(struct temper_type_hid_data){
		.report_length = 8,
		.report_data = { 0x52, 0, 0, 0, 0, 0, 0, 0 }
	};
	
	// Get the device ID and calibration data from the device.
	if ( !temper_type_hid_read_sensors( device ) )
	{
		temper_type_hid_close( device );
		if ( device->error != NULL )
		{
			free( device->error );
		}
		device->error = "Failed to read the device ID from the device.";
		return false;
	}
	
	// The device ID is at the second byte of the read data.
	if ( device_data->data_length < 2 )
	{
		temper_type_hid_close( device );
		device->error = "Data from the device did not include the device ID.";
		return false;
	}
	unsigned char device_id = device_data->data[1];
	
	// Find the subtype that has that device ID.
	struct temper_type_hid_1130_660c_data* subtype =
		(struct temper_type_hid_1130_660c_data*) device->type->data;
	
	while ( subtype->name != NULL && subtype->id != device_id )
	{
		subtype++;
	}
	
	if ( subtype->name == NULL || subtype->id != device_id )
	{
		// Subtype was not found.
		temper_type_hid_close( device );
		device->error = "Unknown device subtype.";
		return false;
	}
	
	// Subtype was found; initialize the device structure with its data.
	device_data->dev_info = &(subtype->data);
	
	// calibration is in offsets 2,3 for internal and 4,5 for external sensor
	
	// for TEMPer1, 2 and NTC, set configuration by writing command 0x43
	return true;
}
