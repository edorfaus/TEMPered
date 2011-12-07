#include <stdio.h>
#include <string.h>
#include "hidapi.h"
#include "temper_type.h"

#define DATA_MAX_LENGTH 64

void print_temp( temper_type *type, char *dev_path )
{
	hid_device* dev;
	unsigned char data[DATA_MAX_LENGTH];
	int size;
	
	dev = hid_open_path( dev_path );
	if ( !dev )
	{
		fprintf( stderr, "Could not open device: %s\n", dev_path );
		return;
	}
	size = hid_write( dev, type->temp_report, type->temp_report_length );
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
		else if (
			size <= type->temperature_high_byte_offset ||
			size <= type->temperature_low_byte_offset
		)
		{
			fprintf( stderr, "Not enough data was read from the sensor.\n" );
		}
		else
		{
			int temp = ( data[type->temperature_low_byte_offset] & 0xFF )
				+ ( (signed char)data[type->temperature_high_byte_offset] << 8 )
			;
			float tempC = temp * 125.0 / 32000.0;
			
			if ( type->has_humidity )
			{
				int rh = ( data[type->humidity_low_byte_offset] & 0xFF )
					+ ( ( data[type->humidity_high_byte_offset] & 0xFF ) << 8 )
				;
				float relhum = -2.0468 + 0.0367 * rh - 1.5955e-6 * rh * rh;
				relhum = ( tempC - 25 ) * ( 0.01 + 0.00008 * rh ) + relhum;
				
				printf(
					"%s: temperature %.2f°C, humidity %.2f%%\n",
					dev_path, tempC, relhum
				);
			}
			else
			{
				printf( "%s: temperature %.2f°C\n", dev_path, tempC );
			}
		}
	}
	hid_close( dev );
}

void print_temp_one( struct hid_device_info *devs, char *dev_path )
{
	struct hid_device_info *info;
	for ( info = devs; info; info = info->next )
	{
		if ( strcmp( info->path, dev_path ) == 0 )
		{
			temper_type *type = get_temper_type( info );
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
				print_temp( type, info->path );
			}
			return;
		}
	}
	fprintf( stderr, "Device info not found for %s\n", dev_path );
}

void print_temp_all( struct hid_device_info *devs )
{
	struct hid_device_info *info;
	for ( info = devs; info; info = info->next )
	{
		temper_type *type = get_temper_type( info );
		if ( type != NULL && !type->ignored )
		{
			print_temp( type, info->path );
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
				print_temp_one( devs, argv[i] );
			}
		}
		else
		{
			print_temp_all( devs );
		}
		hid_free_enumeration( devs );
	}
	if ( hid_exit() != 0 )
	{
		fprintf( stderr, "Error shutting down the HID API.\n" );
	}
	return 0;
}
