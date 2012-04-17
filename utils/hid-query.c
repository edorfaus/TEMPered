#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <hidapi.h>

#define DATA_MAX_LENGTH 256

/** Dump the given data to stdout. */
void dump_data( char* name, unsigned char* data, int data_length )
{
	printf( "%s (%i bytes):", name, data_length );
	int i;
	for ( i = 0 ; i < data_length ; i++ )
	{
		if ( i % 16 == 0 )
		{
			printf( "\n\t" );
		}
		else if ( i % 4 == 0 )
		{
			printf( "  " );
		}
		printf( " %02x", data[i] );
	}
	printf( "\n" );
}

/** Read from the given device with the given timeout. */
int read_from_device( hid_device *dev, int timeout )
{
	unsigned char read_data[DATA_MAX_LENGTH];
	int size;
	size = hid_read_timeout( dev, read_data, sizeof( read_data ), timeout );
	if ( size < 0 )
	{
		fprintf(
			stderr,
			"Read of data from the device failed: %ls\n",
			hid_error( dev )
		);
		return 7;
	}
	else if ( size == 0 )
	{
		return 8;
	}
	else
	{
		if ( size == sizeof( read_data ) )
		{
			printf(
				"Warning: data buffer full, may have lost some data.\n\n"
			);
		}
		dump_data( "Response from device", read_data, size );
	}
	return 0;
}

/** Query the given device with the given data. */
int query_device(
	struct hid_device_info *info, unsigned char *write_data, int write_length
) {
	hid_device* dev;
	int size;
	
	printf( "Device %s : %04hx:%04hx interface %d : %ls %ls\n\n",
		info->path,
		info->vendor_id, info->product_id,
		info->interface_number,
		info->manufacturer_string, info->product_string
	);
	
	dev = hid_open_path( info->path );
	if ( !dev )
	{
		fprintf( stderr, "Could not open device: %s\n", info->path );
		return 5;
	}
	
	int ret = 0;
	
	dump_data( "Writing data", write_data, write_length );
	size = hid_write( dev, write_data, write_length );
	printf( "\n" );
	if ( size <= 0 )
	{
		fprintf( stderr, "Write failed: %ls\n", hid_error( dev ) );
		ret = 6;
	}
	else
	{
		ret = read_from_device( dev, 4000 );
		if ( ret == 8 )
		{
			fprintf( stderr, "No data was read from the device (timeout).\n" );
		}
		else if ( ret == 0 )
		{
			while ( ret == 0 )
			{
				ret = read_from_device( dev, 200 );
			}
			if ( ret == 8 )
			{
				ret = 0;
			}
		}
	}
	hid_close( dev );
	return ret;
}

/** Find a HID device by its path, and query it with the given data. */
int query_device_by_path(
	char *device_path, unsigned char *write_data, int write_length
) {
	if ( hid_init() != 0 )
	{
		fprintf( stderr, "Could not initialize the HID API.\n" );
		return 2;
	}
	int ret = 4;
	struct hid_device_info *devs = hid_enumerate( 0, 0 );
	if ( !devs )
	{
		fprintf( stderr, "No HID devices were found.\n" );
		ret = 3;
	}
	else
	{
		struct hid_device_info *info;
		for ( info = devs; info; info = info->next )
		{
			if ( strcmp( info->path, device_path ) == 0 )
			{
				// found it
				ret = query_device( info, write_data, write_length );
				break;
			}
		}
		if ( ret == 4 )
		{
			fprintf( stderr, "Device not found: %s\n", device_path );
		}
		hid_free_enumeration( devs );
	}
	if ( hid_exit() != 0 )
	{
		fprintf( stderr, "Error shutting down the HID API.\n" );
		ret = 2;
	}
	return ret;
}

/** Enumerate the connected HID devices. */
int enumerate()
{
	if ( hid_init() != 0 )
	{
		fprintf( stderr, "Could not initialize the HID API.\n" );
		return 2;
	}
	int ret = 0;
	struct hid_device_info *devs = hid_enumerate( 0, 0 );
	if ( !devs )
	{
		fprintf( stderr, "No HID devices were found.\n" );
		ret = 3;
	}
	else
	{
		struct hid_device_info *info;
		for ( info = devs; info; info = info->next )
		{
			printf( "%s : %04hx:%04hx interface %d : %ls %ls\n",
				info->path,
				info->vendor_id, info->product_id,
				info->interface_number,
				info->manufacturer_string, info->product_string
			);
		}
		hid_free_enumeration( devs );
	}
	if ( hid_exit() != 0 )
	{
		fprintf( stderr, "Error shutting down the HID API.\n" );
		ret = 2;
	}
	return ret;
}

/** Show a help message about how to use this program. */
int show_help( char* error_message )
{
	if ( error_message != NULL )
	{
		fprintf( stderr, "%s\n", error_message );
	}
	fprintf( stderr,
"Usage: hid-query --enum|-e\n"
" or  : hid-query --help|-h\n"
" or  : hid-query <device> [-<datalen>] [-r <report-id>] <byte>[ <byte>...]\n"
"\n"
"The first form enumerates the HID devices that are attached to the system.\n"
"The second form prints this help message to stderr.\n"
"The third form writes the given bytes to the given HID device, and waits for\n"
"an answer, printing it to stdout. The optional datalen is an integer that\n"
"specifies the number of bytes to write to the device (defaults to the higher\n"
"of 8 and the number of bytes given, with the data zero-padded as necessary).\n"
"The highest allowed data length is %i.\n"
"The report ID will be the first byte written to the device, and if -r is not\n"
"given will be defaulted to zero, as that is the value to use for devices\n"
"that don't use numbered reports (such as the TEMPered devices) - so expect\n"
"to see one more byte than you specified being written to the device.\n",
		DATA_MAX_LENGTH
	);
	return 1;
}

/** Entry point: parse parameters and call the appropriate functions. */
int main( int argc, char** argv )
{
	int i;
	if ( argc < 2 )
	{
		return show_help( "Too few parameters." );
	}
	for ( i = 1; i < argc ; i++ )
	{
		if ( strcmp( argv[i], "--help" ) == 0 || strcmp( argv[i], "-h" ) == 0 )
		{
			return show_help( NULL );
		}
	}
	if ( strcmp( argv[1], "--enum" ) == 0 || strcmp( argv[1], "-e" ) == 0 )
	{
		if ( argc > 2 )
		{
			return show_help( "Too many parameters for --enum" );
		}
		return enumerate();
	}
	if ( argc < 3 )
	{
		return show_help( "Too few parameters." );
	}
	char *device = argv[1];
	long data_length = 8;
	int start = 2;
	int has_report_id = 0;
	if ( argv[start][0] == '-' && argv[start][1] != 'r' )
	{
		if ( argv[start][1] == '\0' )
		{
			return show_help( "Invalid data length specified." );
		}
		char *endptr;
		data_length = strtol( &(argv[start][1]), &endptr, 0 );
		if (
			*endptr != '\0' || data_length < 1 || data_length > DATA_MAX_LENGTH
		) {
			return show_help( "Invalid data length specified." );
		}
		start++;
	}
	if ( argc - start > 0 && strcmp( argv[start], "-r" ) == 0 )
	{
		has_report_id = 1;
		start++;
	}
	if ( argc - start < 1 + has_report_id )
	{
		return show_help( "Too few parameters." );
	}
	if ( argc - start > data_length )
	{
		data_length = argc - start;
	}
	if ( data_length > DATA_MAX_LENGTH )
	{
		return show_help(
			"Too many parameters - too much data to send at once."
		);
	}
	data_length++;
	unsigned char data[data_length];
	memset( data, 0, data_length );
	int offset = 1 - has_report_id;
	for ( i = start ; i < argc ; i++ )
	{
		char *endptr;
		long tmp = strtol( argv[i], &endptr, 0 );
		if ( argv[i][0] == '\0' || *endptr != '\0' || tmp < 0 || tmp > 255 )
		{
			fprintf(
				stderr, "Invalid data byte specified in argument %i: %s\n",
				i, argv[i]
			);
			return show_help( NULL );
		}
		data[offset] = (unsigned char) tmp;
		offset++;
	}
	return query_device_by_path( device, data, data_length );
}
