#include <stdio.h>

#include "hidapi.h"

void print_temp( char* dev_path )
{
	hid_device* dev;
	unsigned char data[64];
	int size;
	
	dev = hid_open_path( dev_path );
	if ( !dev )
	{
		fprintf( stderr, "Could not open device: %s\n", dev_path );
		return;
	}
	data[0] = 1; // The first byte is the report number.
	data[1] = 0x80;
	data[2] = 0x33;
	data[3] = 1;
	data[4] = 0;
	data[5] = 0;
	data[6] = 0;
	data[7] = 0;
	size = hid_write( dev, data, 8 );
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
			fprintf(
				stderr,
				"No data was read from the sensor (timeout).\n"
			);
		}
		else if ( size < 4 )
		{
			fprintf(
				stderr, "Not enough data read from the sensor.\n"
			);
		}
		else
		{
			int temp = ( data[3] & 0xFF )
				+ ( (signed char)data[2] << 8 )
			;
			float tempC = temp * 125.0 / 32000.0;
			
			printf( "Temperature: %.2f C\n", tempC );
		}
	}
	hid_close( dev );
}

void print_temp_all( void )
{
	struct hid_device_info *devs, *info;
	
	devs = hid_enumerate( 0x0c45, 0x7401 );
	if ( !devs )
	{
		fprintf( stderr, "No TEMPerV1.2 devices were found.\n" );
		return;
	}
	for ( info = devs; info; info = info->next )
	{
		if (
			info->vendor_id == 0x0c45 &&
			info->product_id == 0x7401 &&
			info->interface_number == 1
		)
		{
			print_temp( info->path );
		}
	}
	hid_free_enumeration( devs );
}

int main( int argc, char** argv )
{
	if ( hid_init() != 0 )
	{
		fprintf( stderr, "Could not initialize the HID API.\n" );
		return 1;
	}
	if ( argc > 1 )
	{
		int i;
		for ( i = 1; i < argc ; i++ )
		{
			print_temp( argv[i] );
		}
	}
	else
	{
		print_temp_all();
	}
	if ( hid_exit() != 0 )
	{
		fprintf( stderr, "Error shutting down the HID API.\n" );
	}
	return 0;
}
