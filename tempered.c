#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "hidapi.h"

#include "tempered.h"

#include "temper_type.h"

struct tempered_device_ {
	/** The type of temper this device is. */
	struct temper_type const *type;
	
	/** The path for this device. */
	char *path;
	
	/** The last error that occurred with this device. */
	char *error;
	
	/** HID device pointer for this device.
	 * TODO: move this into type-specific data.
	 */
	hid_device *hid_dev;
};

/** This define is used for convenience to set the error message only when the
 * pointer to it is not NULL, and depends on the parameter being named "error".
 */
#define set_error( error_msg ) do { \
		if ( error != NULL ) { *error = error_msg; } \
	} while (false)

/** Set the last error message on the given device. */
static void set_device_error( tempered_device *device, char *error )
{
	if ( device->error != NULL )
	{
		free( device->error );
	}
	device->error = error;
}

/** Initialize the TEMPered library. */
bool tempered_init( char **error )
{
	if ( hid_init() != 0 )
	{
		set_error( "Could not initialize the HID API." );
		return false;
	}
	return true;
}

/** Finalize the TEMPered library. */
bool tempered_exit( char **error )
{
	if ( hid_exit() != 0 )
	{
		set_error( "Error shutting down the HID API." );
		return false;
	}
	return true;
}

/** Enumerate the TEMPer devices. */
struct tempered_device_list* tempered_enumerate( char **error )
{
	struct tempered_device_list *list = NULL, *current = NULL;
	struct hid_device_info *devs, *info;
	devs = hid_enumerate( 0, 0 );
	if ( devs == NULL )
	{
		set_error( "No HID devices were found." );
		return NULL;
	}
	for ( info = devs; info; info = info->next )
	{
		temper_type* type = get_temper_type( info );
		if ( type != NULL && !type->ignored )
		{
			printf(
				"Device %04hx:%04hx if %d rel %4hx | %s | %ls %ls\n",
				info->vendor_id, info->product_id,
				info->interface_number, info->release_number,
				info->path,
				info->manufacturer_string, info->product_string
			);
			struct tempered_device_list *next = malloc(
				sizeof( struct tempered_device_list )
			);
			if ( next == NULL )
			{
				tempered_free_device_list( list );
				set_error( "Unable to allocate memory for list." );
				return NULL;
			}
			
			next->next = NULL;
			next->path = strdup( info->path );
			next->internal_data = type;
			next->type_name = type->name;
			next->vendor_id = info->vendor_id;
			next->product_id = info->product_id;
			next->interface_number = info->interface_number;
			
			if ( next->path == NULL )
			{
				free( next );
				tempered_free_device_list( list );
				set_error( "Unable to allocate memory for path." );
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

/** Open a given device from the list. */
tempered_device* tempered_open( struct tempered_device_list *list, char **error )
{
	tempered_device *device = malloc( sizeof( tempered_device ) );
	if ( device == NULL )
	{
		set_error( "Could not allocate memory for the device." );
		return NULL;
	}
	device->type = (temper_type *)list->internal_data;
	device->error = NULL;
	device->path = strdup( list->path );
	if ( device->path == NULL )
	{
		set_error( "Could not allocate memory for the path." );
		free( device );
		return NULL;
	}
	// TODO: move this into temper_type->open()
	device->hid_dev = hid_open_path( list->path );
	if ( !device->hid_dev )
	{
		set_error( "Could not open the HID device." );
		free( device->path );
		free( device );
		return NULL;
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
	// TODO: move this into temper_type->close()
	hid_close( device->hid_dev );
	// ... but keep the rest here.
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
	return device->type->name;
}

bool tempered_get_temperature( tempered_device *device, float *tempC )
{
	if ( device == NULL )
	{
		return false;
	}
	if ( tempC == NULL )
	{
		set_device_error( device, "The tempC parameter cannot be NULL." );
		return false;
	}
	
	// TODO: Move this into temper_type->read_temp()
	
	struct temper_type const *type = device->type;
	hid_device *dev = device->hid_dev;
	unsigned char data[64];
	int size;
	
	size = hid_write( dev, type->temp_report, type->temp_report_length );
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
		set_device_error( device, error );
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
		set_device_error( device, error );
		return false;
	}
	if ( size == 0 )
	{
		set_device_error(
			device, "No data was read from the sensor (timeout)."
		);
		return false;
	}
	if (
		size <= type->temperature_high_byte_offset ||
		size <= type->temperature_low_byte_offset
	)
	{
		set_device_error( device, "Not enough data was read from the sensor." );
		return false;
	}
	
	// This calculation is based on the FM75 datasheet, and converts
	// from two separate data bytes to a single integer, which is
	// needed for all currently supported temperature sensors.
	int temp = ( data[type->temperature_low_byte_offset] & 0xFF )
		+ ( (signed char)data[type->temperature_high_byte_offset] << 8 )
	;
	
	// This is the same as dividing by 256; basically moving the
	// decimal point into place.
	// This formula is from the FM75 datasheet.
	*tempC = temp * 125.0 / 32000.0;
	
	return true;
}
