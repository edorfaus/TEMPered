#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "hidapi.h"

#include "tempered.h"

struct tempered_device_ {
	hid_device *hid_dev;
	struct temper_type const *type;
};

/** Holds the message for the last error that occurred.
 * @see set_error()
 * @see tempered_get_error()
 */
static const char *tempered_last_error = NULL;

/** Set that an error occurred, with a message.
 * Call with NULL to clear any previous error.
 */
static void set_error( const char *message )
{
	tempered_last_error = message;
}

/** Get the error message for the last error that occurred. */
const char *tempered_error( void )
{
	return tempered_last_error;
}

/** Initialize the TEMPered library. */
bool tempered_init( void )
{
	if ( hid_init() != 0 )
	{
		set_error( "Could not initialize the HID API." );
		return false;
	}
	set_error( NULL );
	return true;
}

/** Finalize the TEMPered library. */
bool tempered_exit( void )
{
	if ( hid_exit() != 0 )
	{
		set_error( "Error shutting down the HID API." );
		return false;
	}
	set_error( NULL );
	return true;
}

/** Enumerate the TEMPer devices. */
tempered_device_list* tempered_enumerate( void )
{
	tempered_device_list *list = NULL, *current = NULL;
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
			next->type = type;
			
			if ( next->path == NULL )
			{
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
		struct tempered_device_list next = list->next;
		free( list->path );
		free( list );
		list = next;
	}
}

/** Open a given device from the list. */
tempered_device* tempered_open( struct tempered_device_list *list )
{
	// TODO: move this into temper_type->open()
	tempered_device *device = malloc( sizeof( tempered_device ) );
	if ( device == NULL )
	{
		set_error( "Could not allocate memory for the device." );
		return NULL;
	}
	device->type = list->type;
	device->hid_dev = hid_open_path( list->path );
	if ( !device->hid_dev )
	{
		set_error( "Could not open the HID device." );
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
	free( device );
}

/** Get the type name of the given device. */
char const * tempered_get_type_name( tempered_device *device )
{
	if ( device == NULL || device->type == NULL || device->type->name == NULL )
	{
		set_error( "Invalid device handle." );
		return NULL;
	}
	return device->type->name;
}
