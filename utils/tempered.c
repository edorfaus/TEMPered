#include <stdio.h>
#include <string.h>
#include <math.h>
#include <tempered.h>

/** Calculate the dew point for the given temperature and relative humidity. */
double get_dew_point( float tempC, float rel_hum )
{
	// This is based on the Sensirion SHT1x datasheet, with some extra reading
	// on Wikipedia.
	double Tn = 243.12;
	double m = 17.62;
	if ( tempC < 0 )
	{
		Tn = 272.62;
		m = 22.46;
	}
	double gamma = log( rel_hum / 100 ) + m * tempC / ( Tn + tempC );
	double dew_point = Tn * gamma / ( m - gamma );
	return dew_point;
}

/** Get and print the sensor values for a given device and sensor. */
void print_device_sensor( tempered_device *device, int sensor )
{
	float tempC, rel_hum;
	int type = tempered_get_sensor_type( device, sensor );
	if ( type & TEMPERED_SENSOR_TYPE_TEMPERATURE )
	{
		if ( !tempered_get_temperature( device, sensor, &tempC ) )
		{
			fprintf(
				stderr, "%s %i: Failed to get the temperature: %s\n",
				tempered_get_device_path( device ), sensor,
				tempered_error( device )
			);
			type &= ~TEMPERED_SENSOR_TYPE_TEMPERATURE;
		}
	}
	if ( type & TEMPERED_SENSOR_TYPE_HUMIDITY )
	{
		if ( !tempered_get_humidity( device, sensor, &rel_hum ) )
		{
			fprintf(
				stderr, "%s %i: Failed to get the humidity: %s\n",
				tempered_get_device_path( device ), sensor,
				tempered_error( device )
			);
			type &= ~TEMPERED_SENSOR_TYPE_HUMIDITY;
		}
	}
	if (
		( type & TEMPERED_SENSOR_TYPE_TEMPERATURE ) &&
		( type & TEMPERED_SENSOR_TYPE_HUMIDITY )
	) {
		printf(
			"%s %i: temperature %.2f°C"
				", relative humidity %.1f%%"
				", dew point %.1f°C\n",
			tempered_get_device_path( device ), sensor,
			tempC, rel_hum, get_dew_point( tempC, rel_hum )
		);
	}
	else if ( type & TEMPERED_SENSOR_TYPE_TEMPERATURE )
	{
		printf(
			"%s %i: temperature %.2f°C\n",
			tempered_get_device_path( device ), sensor,
			tempC
		);
	}
	else if ( type & TEMPERED_SENSOR_TYPE_HUMIDITY )
	{
		printf(
			"%s %i: relative humidity %.1f%%\n",
			tempered_get_device_path( device ), sensor,
			rel_hum
		);
	}
	else
	{
		printf(
			"%s %i: no sensor data available\n",
			tempered_get_device_path( device ), sensor
		);
	}
}

/** Print the sensor values for a given device. */
void print_device( struct tempered_device_list *dev )
{
	char *error = NULL;
	tempered_device *device = tempered_open( dev, &error );
	if ( device == NULL )
	{
		fprintf(
			stderr, "%s: Could not open device: %s\n",
			dev->path, error
		);
		free( error );
		return;
	}
	if ( !tempered_read_sensors( device ) )
	{
		fprintf(
			stderr, "%s: Failed to read the sensors: %s\n",
			tempered_get_device_path( device ),
			tempered_error( device )
		);
	}
	else
	{
		int sensor, sensors = tempered_get_sensor_count( device );
		for ( sensor = 0; sensor < sensors; sensor++ )
		{
			print_device_sensor( device, sensor );
		}
	}
	tempered_close( device );
}

int main( int argc, char **argv )
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
		fprintf( stderr, "Failed to enumerate devices: %s\n", error );
		free( error );
	}
	else
	{
		if ( argc > 1 )
		{
			// We have parameters, so only print those devices that are given.
			int i;
			for ( i = 1; i < argc ; i++ )
			{
				bool found = false;
				struct tempered_device_list *dev;
				for ( dev = list ; dev != NULL ; dev = dev->next )
				{
					if ( strcmp( dev->path, argv[i] ) == 0 )
					{
						found = true;
						print_device( dev );
						break;
					}
				}
				if ( !found )
				{
					fprintf(
						stderr, "%s: TEMPered device not found or ignored.\n",
						argv[i]
					);
				}
			}
		}
		else
		{
			// We don't have any parameters, so print all the devices we found.
			struct tempered_device_list *dev;
			for ( dev = list ; dev != NULL ; dev = dev->next )
			{
				print_device( dev );
			}
		}
		tempered_free_device_list( list );
	}
	
	if ( !tempered_exit( &error ) )
	{
		fprintf( stderr, "%s\n", error );
		free( error );
		return 1;
	}
	return 0;
}
