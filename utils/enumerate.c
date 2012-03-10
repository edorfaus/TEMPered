#include <stdio.h>

#include "tempered.h"

void print_error( char *error )
{
	fprintf( stderr, "%s\n", error );
	free( error );
}

int main( int argc, char **argv )
{
	char *error = NULL;
	if ( !tempered_init( &error ) )
	{
		print_error( error );
		return 1;
	}
	
	struct tempered_device_list *list = tempered_enumerate( &error );
	if ( list == NULL )
	{
		print_error( error );
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
	
	if ( !tempered_exit( &error ) )
	{
		print_error( error );
		return 1;
	}
	return 0;
}
