#include <stdlib.h>
#include <stdio.h>
#include <tempered.h>

/**
This example shows how to enumerate the attached devices.
*/

int main( void )
{
	char error[256];
	if ( !tempered_init( error, sizeof(error) ) )
	{
		fprintf( stderr, "%s\n", error );
		return 1;
	}
	
	struct tempered_device_list *list = tempered_enumerate( error, sizeof(error) );
	if ( list == NULL )
	{
		fprintf( stderr, "%s\n", error );
	}
	else
	{
		struct tempered_device_list *dev;
		for ( dev = list ; dev != NULL ; dev = dev->next )
		{
			printf(
				"Found device: %04x:%04x %i | %s | %s\n",
				dev->vendor_id, dev->product_id,
				dev->interface_number,
				dev->path,
				dev->type_name
			);
		}
		
		tempered_free_device_list( list );
	}
	
	if ( !tempered_exit( error, sizeof(error) ) )
	{
		fprintf( stderr, "%s\n", error );
		return 1;
	}
	return 0;
}
