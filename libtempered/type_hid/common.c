#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <hidapi.h>

#include "common.h"
#include "type-info.h"
#include "internal.h"

#include "../tempered.h"
#include "../tempered-internal.h"

/** Initialize the HID TEMPer types. */
bool temper_type_hid_init( char **error )
{
	if ( hid_init() != 0 )
	{
		if ( error != NULL )
		{
			*error = strdup( "Could not initialize the HID API." );
		}
		return false;
	}
	return true;
}

/** Finalize the HID TEMPer types. */
bool temper_type_hid_exit( char **error )
{
	if ( hid_exit() != 0 )
	{
		if ( error != NULL )
		{
			*error = strdup( "Error shutting down the HID API." );
		}
		return false;
	}
	return true;
}

/** Enumerate the HID TEMPer devices. */
struct tempered_device_list* temper_type_hid_enumerate( char **error )
{
	struct tempered_device_list *list = NULL, *current = NULL;
	struct hid_device_info *devs, *info;
	devs = hid_enumerate( 0, 0 );
	if ( devs == NULL )
	{
		if ( error != NULL )
		{
			*error = strdup( "No HID devices were found." );
		}
		return NULL;
	}
	for ( info = devs; info; info = info->next )
	{
		struct temper_type* type = temper_type_find(
			info->vendor_id, info->product_id, info->interface_number
		);
		if ( type != NULL && type->open != NULL )
		{
			#ifdef DEBUG
			printf(
				"Device %04hx:%04hx if %d rel %4hx | %s | %ls %ls\n",
				info->vendor_id, info->product_id,
				info->interface_number, info->release_number,
				info->path,
				info->manufacturer_string, info->product_string
			);
			#endif
			struct tempered_device_list *next = malloc(
				sizeof( struct tempered_device_list )
			);
			if ( next == NULL )
			{
				tempered_free_device_list( list );
				if ( error != NULL )
				{
					*error = strdup( "Unable to allocate memory for list." );
				}
				return NULL;
			}
			
			next->next = NULL;
			next->path = strdup( info->path );
			next->type_name = type->name;
			next->vendor_id = info->vendor_id;
			next->product_id = info->product_id;
			next->interface_number = info->interface_number;
			
			if ( next->path == NULL )
			{
				free( next );
				tempered_free_device_list( list );
				if ( error != NULL )
				{
					*error = strdup( "Unable to allocate memory for path." );
				}
				return NULL;
			}
			
			if ( current == NULL )
			{
				list = next;
				current = list;
			}
			else
			{
				current->next = next;
				current = current->next;
			}
		}
	}
	hid_free_enumeration( devs );
	return list;
}

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
	device_data->group_data = NULL;
	device_data->hid_dev = hid_open_path( device->path );
	if ( device_data->hid_dev == NULL )
	{
		free( device_data );
		tempered_set_error( device, strdup( "Failed to open HID device." ) );
		return false;
	}
	device->data = device_data;
	return true;
}

void temper_type_hid_close( tempered_device* device )
{
	struct temper_type_hid_device_data *device_data =
		(struct temper_type_hid_device_data *) device->data;
	hid_close( device_data->hid_dev );
	if ( device_data->group_data != NULL )
	{
		free( device_data->group_data );
	}
	free( device_data );
}

bool temper_type_hid_subtype_open( tempered_device* device )
{
	struct temper_type_hid_device_data *device_data =
		(struct temper_type_hid_device_data *) device->data;
	
	int group_count =
		((struct temper_subtype_hid *) device->subtype)->sensor_group_count;
	
	device_data->group_data = malloc(
		group_count * sizeof( struct tempered_type_hid_query_result )
	);
	if ( device_data->group_data == NULL )
	{
		tempered_set_error(
			device, strdup( "Failed to allocate memory for the group data." )
		);
		return false;
	}
	int i;
	for ( i = 0; i < group_count ; i++ )
	{
		device_data->group_data[i].length = 0;
	}
	return true;
}

/** Method for getting the subtype ID from HID devices. */
bool temper_type_hid_get_subtype_id(
	tempered_device* device, unsigned char* subtype_id
) {
	struct tempered_type_hid_subtype_data *subtype_data =
		(struct tempered_type_hid_subtype_data*) device->type->get_subtype_data;
	
	if ( subtype_data == NULL )
	{
		// We don't have the necessary data, so pretend we got subtype 0.
		*subtype_id = 0;
		return true;
	}
	
	struct tempered_type_hid_query_result result;
	
	if ( !temper_type_hid_query( device, &subtype_data->query, &result ) )
	{
		return false;
	}
	
	if ( result.length <= subtype_data->id_offset )
	{
		tempered_set_error(
			device, strdup( "Not enough data was read from the device." )
		);
		return false;
	}
	
	*subtype_id = result.data[subtype_data->id_offset];
	
	return true;
}

bool temper_type_hid_read_sensors( tempered_device* device )
{
	struct temper_subtype_hid *subtype =
		(struct temper_subtype_hid *) device->subtype;
	
	struct temper_type_hid_device_data *device_data =
		(struct temper_type_hid_device_data *) device->data;
	
	int i;
	for ( i = 0; i < subtype->sensor_group_count ; i++ )
	{
		struct tempered_type_hid_sensor_group *group =
			&subtype->sensor_groups[i];
		
		struct tempered_type_hid_query_result *group_data =
			&device_data->group_data[i];
		
		if ( !group->read_sensors( device, group, group_data ) )
		{
			return false;
		}
	}
	return true;
}

bool temper_type_hid_read_sensor_group(
	tempered_device* device, struct tempered_type_hid_sensor_group* group,
	struct tempered_type_hid_query_result* group_data
) {
	return temper_type_hid_query( device, &group->query, group_data );
}


bool temper_type_hid_query(
	tempered_device* device, struct tempered_type_hid_query* query,
	struct tempered_type_hid_query_result* result
) {
	struct temper_type_hid_device_data *device_data =
		(struct temper_type_hid_device_data *) device->data;
	
	hid_device *hid_dev = device_data->hid_dev;
	
	int size = hid_write( hid_dev, query->data, query->length );
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
		result->length = 0;
		return false;
	}
	size = hid_read_timeout(
		hid_dev, result->data, sizeof( result->data ), 1000
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
		result->length = 0;
		return false;
	}
	result->length = size;
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
/*
	struct temper_type_hid_device_data *device_data =
		(struct temper_type_hid_device_data *) device->data;
	
	struct temper_type_hid_data *info = device_data->dev_info;
	
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
*/
	return true;
}

bool temper_type_hid_get_humidity(
	tempered_device* device, int sensor, float* rel_hum
) {
/*
	struct temper_type_hid_device_data *device_data =
		(struct temper_type_hid_device_data *) device->data;
	
	struct temper_type_hid_data *info = device_data->dev_info;
	
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
*/
	return true;
}
