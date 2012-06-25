#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tempered.h>

/**
This example shows how to enumerate the attached devices, print some
information about them, and read their sensors once.
*/

/** Get and print the sensor values for a given device and sensor. */
void read_device_sensor( tempered_device *device, int sensor )
{
	printf( "\tSensor %i:\n", sensor );
	int type = tempered_get_sensor_type( device, sensor );
	if ( type == TEMPERED_SENSOR_TYPE_NONE )
	{
		printf( "\t\tNo such sensor, or type is not supported.\n" );
	}
	if ( type & TEMPERED_SENSOR_TYPE_TEMPERATURE )
	{
		float tempC;
		if ( tempered_get_temperature( device, sensor, &tempC ) )
		{
			printf( "\t\tTemperature: %.2f°C\n", tempC );
		}
		else
		{
			printf(
				"\t\tFailed to get the temperature: %s\n",
				tempered_error( device )
			);
		}
	}
	if ( type & TEMPERED_SENSOR_TYPE_HUMIDITY )
	{
		float rel_hum;
		if ( tempered_get_humidity( device, sensor, &rel_hum ) )
		{
			printf( "\t\tHumidity: %.1f%%RH\n", rel_hum );
		}
		else
		{
			printf(
				"\t\tFailed to get the humidity: %s\n",
				tempered_error( device )
			);
		}
	}
}

/** Get and print information and sensor values for a given device. */
void read_device( struct tempered_device_list *dev )
{
	printf(
		"Device %s : USB IDs %04x:%04x, interface %i\n"
			"\tEnumeration type name: %s\n",
		dev->path,
		dev->vendor_id, dev->product_id,
		dev->interface_number,
		dev->type_name
	);
	char *error = NULL;
	tempered_device *device = tempered_open( dev, &error );
	if ( device == NULL )
	{
		printf( "\tOpen failed, error: %s\n", error );
		free( error );
		return;
	}
	printf(
		"\tOpen succeeded.\n"
			"\tDevice path: %s\n"
			"\tDevice type name: %s\n",
		tempered_get_device_path( device ),
		tempered_get_type_name( device )
	);
	if ( !tempered_read_sensors( device ) )
	{
		printf(
			"\tFailed to read the sensors: %s\n",
			tempered_error( device )
		);
	}
	else
	{
		int sensor, sensors = tempered_get_sensor_count( device );
		printf( "\tSensor count: %i\n", sensors );
		for ( sensor = 0; sensor < sensors; sensor++ )
		{
			read_device_sensor( device, sensor );
		}
	}
	tempered_close( device );
}

int main( void )
{
	char *error = NULL;
	if ( !tempered_init( &error ) )
	{
		fprintf( stderr, "Failed to initialize libtempered: %s\n", error );
		free( error );
		return 1;
	}
	
	struct tempered_device_list *list = tempered_enumerate( &error );
	if ( list == NULL )
	{
		if ( error == NULL )
		{
			printf( "No devices were found.\n" );
		}
		else
		{
			fprintf( stderr, "Failed to enumerate devices: %s\n", error );
			free( error );
		}
	}
	else
	{
		struct tempered_device_list *dev;
		for ( dev = list ; dev != NULL ; dev = dev->next )
		{
			read_device( dev );
		}
		tempered_free_device_list( list );
	}
	
	if ( !tempered_exit( &error ) )
	{
		fprintf( stderr, "Failed to shut down libtempered: %s\n", error );
		free( error );
		return 1;
	}
	return 0;
}
