#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "hidapi.h"
#include "temper_type.h"

int main( int argc, char **argv )
{
	struct hid_device_info *devs, *info;
	bool list_all = false;
	int i;
	
	for ( i = 1; i < argc; i++ )
	{
		if ( strcmp( "-a", argv[i] ) == 0 )
		{
			list_all = true;
		}
	}
	
	if ( hid_init() != 0 )
	{
		fprintf( stderr, "Could not initialize the HID API.\n" );
		return 1;
	}
	devs = hid_enumerate( 0, 0 );
	if ( !devs )
	{
		printf( "No HID devices were found.\n" );
	}
	for ( info = devs; info; info = info->next )
	{
		temper_type* type = get_temper_type( info );
		if ( list_all || ( type != NULL && !type->ignored ) )
		{
			printf(
				"Device %04hx:%04hx if %d rel %4hx | %s | %ls %ls\n",
				info->vendor_id, info->product_id,
				info->interface_number, info->release_number,
				info->path,
				info->manufacturer_string, info->product_string
			);
			if ( list_all )
			{
				/* Usage and usage page is only valid on Windows/Mac :(
				if ( info->usage_page != 0 || info->usage != 0 )
				{
					printf(
						"\tUsage page %hd, usage %hd\n",
						info->usage_page, info->usage
					);
				}
				// */
				if ( info->serial_number != NULL )
				{
					printf( "\tSerial number: %ls\n", info->serial_number );
				}
			}
			if ( list_all && type != NULL )
			{
				if ( type->ignored )
				{
					printf(
						"\tRecognized as ignored part of: %s\n", type->name
					);
				}
				else
				{
					printf( "\tRecognized as: %s\n", type->name );
				}
			}
		}
	}
	hid_free_enumeration( devs );
	if ( hid_exit() != 0 )
	{
		fprintf( stderr, "Error shutting down the HID API.\n" );
	}
	return 0;
}
