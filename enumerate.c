#include <stdio.h>
#include <string.h>

#include "hidapi.h"

int main( int argc, char **argv )
{
	struct hid_device_info *devs, *info;
	int list_all = 0, i;
	
	for ( i = 1; i < argc; i++ )
	{
		if ( strcmp( "-a", argv[i] ) == 0 )
		{
			list_all = 1;
		}
	}
	
	if ( hid_init() != 0 )
	{
		fprintf( stderr, "Could not initialize the HID API.\n" );
		return 1;
	}
	if ( list_all != 0 )
	{
		devs = hid_enumerate( 0, 0 );
	}
	else
	{
		devs = hid_enumerate( 0x0c45, 0x7401 );
	}
	if ( !devs )
	{
		printf( "No devices were found.\n" );
	}
	for ( info = devs; info; info = info->next )
	{
		if (
			list_all != 0 ||
			(
				info->vendor_id == 0x0c45 &&
				info->product_id == 0x7401 &&
				info->interface_number == 1
			)
		)
		{
			printf("Device %04hx:%04hx %d | %s | %ls %ls\n",
				info->vendor_id, info->product_id,
				info->interface_number,
				info->path,
				info->manufacturer_string, info->product_string
			);
		}
	}
	hid_free_enumeration( devs );
	if ( hid_exit() != 0 )
	{
		fprintf( stderr, "Error shutting down the HID API.\n" );
	}
	return 0;
}
