#include <stdio.h>

#include "tempered.h"

void print_error( void )
{
	fprintf( stderr, "%s\n", tempered_error() );
}

int main( int argc, char **argv )
{
	if ( !tempered_init() )
	{
		print_error();
		return 1;
	}
	
	struct tempered_device_list *list = tempered_enumerate();
	if ( list == NULL )
	{
		print_error();
	}
	else
	{
		struct tempered_device_list *dev;
		for ( dev = list ; dev != NULL ; dev = dev->next )
		{
			printf(
				"Found device: %04x:%04x %i | %s | %s\n",
				dev->type->vendor_id, dev->type->product_id,
				dev->type->interface_number,
				dev->path,
				dev->type->name
			);
		}
		
		tempered_free_device_list( list );
	}
	
	if ( !tempered_exit() )
	{
		print_error();
		return 1;
	}
	return 0;
}
