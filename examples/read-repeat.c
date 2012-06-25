#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <tempered.h>

/**
This example shows how to open a single device, and read its sensors repeatedly.
*/

/** Get and print the sensor values for a given device and sensor. */
void read_device_sensor( tempered_device *device, int sensor )
{
	printf( "Sensor %i:", sensor );
	int type = tempered_get_sensor_type( device, sensor );
	if ( type == TEMPERED_SENSOR_TYPE_NONE )
	{
		printf( " No such sensor, or type is not supported.\n" );
		return;
	}
	if ( type & TEMPERED_SENSOR_TYPE_TEMPERATURE )
	{
		float tempC;
		if ( tempered_get_temperature( device, sensor, &tempC ) )
		{
			printf( " %.2f°C", tempC );
		}
		else
		{
			printf(
				" temperature failed (%s)",
				tempered_error( device )
			);
		}
	}
	if ( type & TEMPERED_SENSOR_TYPE_HUMIDITY )
	{
		float rel_hum;
		if ( tempered_get_humidity( device, sensor, &rel_hum ) )
		{
			printf( " %.1f%%RH", rel_hum );
		}
		else
		{
			printf(
				" humidity failed (%s)",
				tempered_error( device )
			);
		}
	}
	printf( "\n" );
}

/** Get and print the sensor values for a given device repeatedly. */
void read_repeatedly( tempered_device *device )
{
	int i;
	for ( i = 0; i < 10; i++ )
	{
		if ( i > 0 )
		{
			sleep( 5 );
		}
		if ( !tempered_read_sensors( device ) )
		{
			printf(
				"Failed to read the sensors: %s\n",
				tempered_error( device )
			);
		}
		else
		{
			int sensor, sensors = tempered_get_sensor_count( device );
			for ( sensor = 0; sensor < sensors; sensor++ )
			{
				read_device_sensor( device, sensor );
			}
		}
	}
}

/** Open the device with the given device path. */
tempered_device* open_device( char *dev_path )
{
	char *error = NULL;
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
		return NULL;
	}
	tempered_device *device = NULL;
	bool found = false;
	struct tempered_device_list *dev;
	for ( dev = list ; dev != NULL ; dev = dev->next )
	{
		if ( strcmp( dev->path, dev_path ) == 0 )
		{
			found = true;
			device = tempered_open( dev, &error );
			break;
		}
	}
	tempered_free_device_list( list );
	if ( device == NULL )
	{
		if ( found )
		{
			fprintf(
				stderr, "Opening %s failed, error: %s\n",
				dev_path, error
			);
			free( error );
		}
		else
		{
			fprintf( stderr, "Device not found: %s\n", dev_path );
		}
	}
	return device;
}

int main( int argc, char *argv[] )
{
	if ( argc <= 1 )
	{
		fprintf( stderr, "Usage: read-repeat <device>\n" );
		return 1;
	}
	char *error = NULL;
	if ( !tempered_init( &error ) )
	{
		fprintf( stderr, "Failed to initialize libtempered: %s\n", error );
		free( error );
		return 1;
	}
	
	tempered_device *device = open_device( argv[1] );
	if ( device != NULL )
	{
		read_repeatedly( device );
		tempered_close( device );
	}
	
	if ( !tempered_exit( &error ) )
	{
		fprintf( stderr, "Failed to shut down libtempered: %s\n", error );
		free( error );
		return 1;
	}
	return 0;
}
