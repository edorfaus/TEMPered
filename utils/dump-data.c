#include <stdio.h>
#include <string.h>
#include "hidapi.h"
#include "temper_type.h"
#include "type_hid/common.h"

#define DATA_MAX_LENGTH 256

void dump_data( temper_type *type, struct hid_device_info *info )
{
	hid_device* dev;
	unsigned char data[DATA_MAX_LENGTH];
	int size;
	
	printf( "Trying device: %04hx:%04hx %d | %s | %s | %ls %ls\n",
		info->vendor_id, info->product_id,
		info->interface_number,
		info->path,
		type->name,
		info->manufacturer_string, info->product_string
	);
	dev = hid_open_path( info->path );
	if ( !dev )
	{
		fprintf( stderr, "Could not open device: %s\n", info->path );
		return;
	}
	struct temper_type_hid_data *hid_data =
		(struct temper_type_hid_data *) type->data;
	size = hid_write( dev, hid_data->report_data, hid_data->report_length );
	if ( size <= 0 )
	{
		fprintf( stderr, "Write failed: %ls\n", hid_error( dev ) );
	}
	else
	{
		size = hid_read_timeout( dev, data, sizeof(data), 1000 );
		if ( size < 0 )
		{
			fprintf(
				stderr,
				"Read of data from the sensor failed: %ls\n",
				hid_error( dev )
			);
		}
		else if ( size == 0 )
		{
			fprintf( stderr, "No data was read from the sensor (timeout).\n" );
		}
		else
		{
			if ( size == sizeof(data) )
			{
				printf( "Warning: data buffer full, may have lost some data.\n" );
			}
			printf( "Read %i bytes of data:", size );
			int i;
			for ( i = 0; i < size; i++ )
			{
				if ( ( i % 8 ) == 0 )
				{
					printf( "\n0x%02x:", i );
				}
				printf( " %02x", (unsigned int)(data[i]) );
			}
			printf( "\n\n" );
		}
	}
	hid_close( dev );
}

void dump_data_one( struct hid_device_info *devs, char *dev_path )
{
	struct hid_device_info *info;
	for ( info = devs; info; info = info->next )
	{
		if ( strcmp( info->path, dev_path ) == 0 )
		{
			temper_type *type = temper_type_find(
				info->vendor_id, info->product_id, info->interface_number
			);
			if ( type == NULL )
			{
				fprintf( stderr, "Device type not recognized: %s\n", dev_path );
			}
			else if ( type->ignored )
			{
				fprintf(
					stderr,
					"Device type is ignored (use the other interface): %s\n",
					dev_path
				);
			}
			else
			{
				dump_data( type, info );
			}
			return;
		}
	}
	fprintf( stderr, "Device info not found for %s\n", dev_path );
}

void dump_data_all( struct hid_device_info *devs )
{
	struct hid_device_info *info;
	for ( info = devs; info; info = info->next )
	{
		temper_type *type = temper_type_find(
			info->vendor_id, info->product_id, info->interface_number
		);
		if ( type != NULL && !type->ignored )
		{
			dump_data( type, info );
		}
	}
}

int main( int argc, char** argv )
{
	if ( hid_init() != 0 )
	{
		fprintf( stderr, "Could not initialize the HID API.\n" );
		return 1;
	}
	
	struct hid_device_info *devs = hid_enumerate( 0, 0 );
	if ( !devs )
	{
		fprintf( stderr, "No HID devices were found.\n" );
	}
	else
	{
		if ( argc > 1 )
		{
			int i;
			for ( i = 1; i < argc ; i++ )
			{
				dump_data_one( devs, argv[i] );
			}
		}
		else
		{
			dump_data_all( devs );
		}
		hid_free_enumeration( devs );
	}
	if ( hid_exit() != 0 )
	{
		fprintf( stderr, "Error shutting down the HID API.\n" );
	}
	return 0;
}
