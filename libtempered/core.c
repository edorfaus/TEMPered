#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "tempered.h"
#include "tempered-internal.h"

#include "temper_type.h"

/** Set the last error message on the given device. */
void tempered_set_error( tempered_device *device, char *error )
{
	if ( device == NULL )
	{
		return;
	}
	if ( device->error != NULL )
	{
		free( device->error );
	}
	device->error = error;
}

/** Initialize the TEMPered library. */
bool tempered_init( char **error )
{
	return temper_type_init( error );
}

/** Finalize the TEMPered library. */
bool tempered_exit( char **error )
{
	return temper_type_exit( error );
}

/** Enumerate the TEMPer devices. */
struct tempered_device_list* tempered_enumerate( char **error )
{
	return temper_type_enumerate( error );
}

/** Free the memory used by the given device list. */
void tempered_free_device_list( struct tempered_device_list *list )
{
	while ( list != NULL )
	{
		struct tempered_device_list *next = list->next;
		free( list->path );
		free( list );
		list = next;
	}
}

/** Helper function for open: find the device subtype and set it. */
static bool tempered_open__find_subtype( tempered_device *device )
{
	if ( device->type->get_subtype_id == NULL )
	{
		device->subtype = device->type->subtypes[0];
	}
	else
	{
		unsigned char subtype_id;
		if ( !device->type->get_subtype_id( device, &subtype_id ) )
		{
			if ( device->error == NULL )
			{
				tempered_set_error(
					device,
					strdup( "Getting the subtype ID from the device failed." )
				);
			}
			return false;
		}
		device->subtype = temper_type_find_subtype( device->type, subtype_id );
		if ( device->subtype == NULL )
		{
			int size = snprintf(
				NULL, 0, "Unknown device subtype ID: 0x%02x",
				subtype_id
			);
			// TODO: check that size >= 0
			size++;
			char *error = malloc( size );
			size = snprintf(
				error, size, "Unknown device subtype ID: 0x%02x",
				subtype_id
			);
			tempered_set_error( device, error );
			return false;
		}
	}
	return true;
}

/** Open a given device from the list. */
tempered_device* tempered_open( struct tempered_device_list *list, char **error )
{
	if ( list == NULL )
	{
		if ( error != NULL )
		{
			*error = strdup( "Invalid device given." );
		}
		return NULL;
	}
	struct temper_type * type = temper_type_find(
		list->vendor_id, list->product_id, list->interface_number
	);
	if ( type == NULL )
	{
		if ( error != NULL )
		{
			*error = strdup( "Invalid device given (type not found)." );
		}
		return NULL;
	}
	if ( type->open == NULL )
	{
		if ( error != NULL )
		{
			*error = strdup( "Device type has no open method." );
		}
		return NULL;
	}
	tempered_device *device = malloc( sizeof( tempered_device ) );
	if ( device == NULL )
	{
		if ( error != NULL )
		{
			*error = strdup( "Could not allocate memory for the device." );
		}
		return NULL;
	}
	device->type = type;
	device->subtype = NULL;
	device->error = NULL;
	device->data = NULL;
	device->path = strdup( list->path );
	if ( device->path == NULL )
	{
		if ( error != NULL )
		{
			*error = strdup( "Could not allocate memory for the path." );
		}
		free( device );
		return NULL;
	}
	if ( !device->type->open( device ) )
	{
		if ( error != NULL )
		{
			if ( device->error != NULL )
			{
				*error = device->error;
			}
			else
			{
				*error = strdup(
					"Type-specific device open failed with no error message."
				);
			}
		}
		else if ( device->error != NULL )
		{
			free( device->error );
		}
		free( device->path );
		free( device );
		return NULL;
	}
	if ( !tempered_open__find_subtype( device ) )
	{
		if ( error != NULL )
		{
			*error = device->error;
			device->error = NULL;
		}
		tempered_close( device );
		return NULL;
	}
	if ( device->subtype->open != NULL )
	{
		if ( !device->subtype->open( device ) )
		{
			if ( error != NULL )
			{
				if ( device->error != NULL )
				{
					*error = device->error;
					device->error = NULL;
				}
				else
				{
					*error = strdup(
						"Type-specific device open failed with no error message."
					);
				}
			}
			device->subtype = NULL;
			tempered_close( device );
			return NULL;
		}
	}
	return device;
}

/** Close an open device. */
void tempered_close( tempered_device *device )
{
	if ( device == NULL )
	{
		return;
	}
	if ( device->subtype != NULL && device->subtype->close != NULL )
	{
		device->subtype->close( device );
	}
	if ( device->type->close != NULL )
	{
		device->type->close( device );
	}
	if ( device->error != NULL )
	{
		free( device->error );
	}
	free( device->path );
	free( device );
}

/** Get the last error message from an open device. */
char* tempered_error( tempered_device *device )
{
	return device->error;
}

/** Get the device path of the given device. */
char const * tempered_get_device_path( tempered_device *device )
{
	return device->path;
}

/** Get the type name of the given device. */
char const * tempered_get_type_name( tempered_device *device )
{
	return device->subtype->name;
}

/** Get the number of sensors supported by the given device. */
int tempered_get_sensor_count( tempered_device *device )
{
	if ( device == NULL )
	{
		return 0;
	}
	if ( device->subtype->get_sensor_count != NULL )
	{
		return device->subtype->get_sensor_count( device );
	}
	return 1;
}

/** Get the sensor type of the given sensor. */
int tempered_get_sensor_type( tempered_device *device, int sensor )
{
	if ( device == NULL )
	{
		return TEMPERED_SENSOR_TYPE_NONE;
	}
	if ( sensor < 0 || sensor >= tempered_get_sensor_count( device ) )
	{
		tempered_set_error( device, strdup( "Sensor ID is out of range." ) );
		return TEMPERED_SENSOR_TYPE_NONE;
	}
	if ( device->subtype->get_sensor_type != NULL )
	{
		return device->subtype->get_sensor_type( device, sensor );
	}
	if ( device->subtype->get_humidity != NULL )
	{
		return TEMPERED_SENSOR_TYPE_TEMPERATURE | TEMPERED_SENSOR_TYPE_HUMIDITY;
	}
	return TEMPERED_SENSOR_TYPE_TEMPERATURE;
}

/** Read the sensors of the given device. */
bool tempered_read_sensors( tempered_device *device )
{
	if ( device == NULL )
	{
		return false;
	}
	if ( device->subtype->read_sensors == NULL )
	{
		tempered_set_error(
			device, strdup( "This device type cannot read its sensors." )
		);
		return false;
	}
	return device->subtype->read_sensors( device );
}

/** Get the temperature from the given device. */
bool tempered_get_temperature(
	tempered_device *device, int sensor, float *tempC
) {
	if ( device == NULL )
	{
		return false;
	}
	if ( tempC == NULL )
	{
		tempered_set_error(
			device, strdup( "The tempC parameter cannot be NULL." )
		);
		return false;
	}
	if ( sensor < 0 || sensor >= tempered_get_sensor_count( device ) )
	{
		tempered_set_error( device, strdup( "Sensor ID is out of range." ) );
		return false;
	}
	if ( device->subtype->get_temperature == NULL )
	{
		tempered_set_error(
			device, strdup( "This device type cannot read the temperature." )
		);
		return false;
	}
	return device->subtype->get_temperature( device, sensor, tempC );
}

/** Get the relative humidity from the given device. */
bool tempered_get_humidity(
	tempered_device *device, int sensor, float *rel_hum
) {
	if ( device == NULL )
	{
		return false;
	}
	if ( rel_hum == NULL )
	{
		tempered_set_error(
			device, strdup( "The rel_hum parameter cannot be NULL." )
		);
		return false;
	}
	if ( sensor < 0 || sensor >= tempered_get_sensor_count( device ) )
	{
		tempered_set_error( device, strdup( "Sensor ID is out of range." ) );
		return false;
	}
	if ( device->subtype->get_humidity == NULL )
	{
		tempered_set_error(
			device, strdup( "This device type cannot read the humidity." )
		);
		return false;
	}
	return device->subtype->get_humidity( device, sensor, rel_hum );
}
